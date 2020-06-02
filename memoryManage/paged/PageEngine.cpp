#include "PageEngine.h"
#include "../utils/Timer.h"
#include "../journal/PageUtil.h"
#include "../utils/json.hpp"

#include <sstream>
#include <mutex>
#include <boost/bind.hpp>

USING_MM_NAMESPACE

using json = nlohmann::json;

std::mutex paged_mtx;
#define SOURCE_INDEX SOURCE_CTP
#define CONFIGURATION_JSON_FILE OPTIONHEDGE_ROOT_DIR "configFiles/tradingConfig.json"
#define COMM_FILE OPTIONHEDGE_JOURNAL_FOLDER "PAGE_ENGINE_COMM"
const string commFileName = COMM_FILE;
const int INTERVAL_IN_MILLISEC = 1000000;

#define TEMP_PAGE OPTIONHEDGE_JOURNAL_FOLDER "TEMP_PAGE"


LogPtr static_logger;

bool PageEngine::write(string content, byte msg_type, bool is_last, short source)
{
	if (writer.get() == nullptr)
	{
		return false;
	}
	writer->write_frame(content.c_str(),content.length()+1,source,msg_type,is_last,-1);
}

void PageEngine::acquire_mutex() const
{
	paged_mtx.lock();
}

void PageEngine::release_mutex() const
{
	paged_mtx.unlock();
}

PageEngine::PageEngine():commBuffer(nullptr),commFile(commFileName),maxIdx(0),microsecFreq(INTERVAL_IN_MILLISEC),
						task_running(false),comm_running(false)
{
	logger = Log::getLogger("PageEngine");
	static_logger = logger;
}

PageEngine::~PageEngine()
{

}

void PageEngine::start()
{
	OPTIONHEDGE_LOG_INFO(logger, "reset socket: " << PAGED_SOCKET_FILE);
	remove(PAGED_SOCKET_FILE);

	OPTIONHEDGE_LOG_INFO(logger, "loading page buffer: " << commFile);
	commBuffer = PageUtil::LoadPageBuffer(commFile, COMM_SIZE, true, true);

	memset(commBuffer, 0, COMM_SIZE);
	comm_running = false;

	commThread = ThreadPtr(new std::thread(boost::bind(&PageEngine::start_comm,this)));
	socketThread = ThreadPtr(new std::thread(boost::bind(&PageEngine::start_socket,this)));

	while (!(PageSocketHandler::getInstance()->is_running()&&comm_running))
	{
		usleep(INTERVAL_IN_MILLISEC / 10);
	}

	OPTIONHEDGE_LOG_INFO(logger, "creating writer: (f)" << PAGED_JOURNAL_FOLDER << " (n)" << PAGED_JOURNAL_NAME);

	writer = JournalWriter::create(PAGED_JOURNAL_FOLDER, PAGED_JOURNAL_NAME);

	if (microsecFreq <= 0)
		throw std::runtime_error("unaccepted task time interval");
	task_running = true;

	write("", MSG_TYPE_PAGED_START);
}

/*
void PageEngine::set_freq(double secondFreq)
{
	microsecFreq = (int)(secondFreq * MICROSECONDS_PER_SECOND);
	OPTIONHEDGE_LOG_INFO(logger, "microsecond frequency updated to " << microsecFreq);
}
*/
void PageEngine::stop()
{
	OPTIONHEDGE_LOG_INFO(logger,"stop Engine........");

	/* write paged end in system journal */
	write("", MSG_TYPE_PAGED_END);
	writer.reset();

	/* stop task thread first */
	task_running = false;
	/*
	if (taskThread.get() != nullptr)
	{
		taskThread->join();
		taskThread.reset();
	}*/

	comm_running = false;
	if (commThread.get() != nullptr)
	{
		commThread->join();
		commThread.reset();
	}
	
	OPTIONHEDGE_LOG_INFO(logger, "stop Comm done .........");

	/* stop socket io thread */
	PageSocketHandler::getInstance()->stop();
	if (socketThread.get() != nullptr)
	{
		socketThread->join();
		socketThread.reset();
	}

	OPTIONHEDGE_LOG_INFO(logger, "(stopSocket) done");

	/* finish up */
	OPTIONHEDGE_LOG_INFO(logger, "(stop) done");
}

void PageEngine::start_socket()
{
	PageSocketHandler::getInstance()->run(this);
}

int PageEngine::reg_journal(const string& clientName)
{
	size_t idx = 0;
	for (; idx < MAX_COMM_USER_NUMBER; ++idx)
	{
		if (GET_COMM_MSG(commBuffer, idx)->status == PAGED_COMM_RAW)
			break;
	}

	if (idx > MAX_COMM_USER_NUMBER)
	{
		OPTIONHEDGE_LOG_INFO(logger, "can not allcocate idx in commfile");
		return -1;
	}

	if (idx > maxIdx)
	{
		maxIdx = idx;
	}

	PageCommMsg* msg = GET_COMM_MSG(commBuffer, idx);
	msg->status = PAGED_COMM_OCCUPIED;
	msg->last_page_num = 0;
	auto it = clientJournals.find(clientName);

	if (it == clientJournals.end())
	{
		OPTIONHEDGE_LOG_INFO(logger, "can not find the client in reg_journal,check if client reg first");
		return -1;
	}

	it->second.user_index_vec.push_back(idx);
	OPTIONHEDGE_LOG_INFO(logger, "[RegJournal] (client) " << clientName << " (idx)" << idx);
	return idx;
}

bool PageEngine::reg_client(string & _commFile, int& fileSize, const string&clientName, int pid, bool isWriter)
{
	OPTIONHEDGE_LOG_INFO(logger, "[RegClient] (name)" << clientName << " (writer?)" << isWriter);
	if (clientJournals.find(clientName) != clientJournals.end())
	{
		return false;
	}

	map<int, vector<string>>::iterator it = pidClient.find(pid);

	if (it == pidClient.end())
	{
		pidClient[pid] = {clientName};
	}
	else
	{
		pidClient[pid].push_back(clientName);
	}

	PageClientInfo& clientInfo = clientJournals[clientName];
	clientInfo.user_index_vec.clear();
	clientInfo.reg_nano = getNanoTime();
	clientInfo.is_writing = isWriter;
	clientInfo.is_strategy = false;
	clientInfo.rid_start = -1;
	clientInfo.rid_end = -1;
	clientInfo.pid = pid;
	_commFile = this->commFile;
	fileSize = COMM_SIZE;
	return true;
}

void PageEngine::release_page(const PageCommMsg& msg)
{
	OPTIONHEDGE_LOG_INFO(logger,"[RmPage](folder)"<<msg.folder<<" and jname is "<<msg.name<<" PageNum is "<<msg.page_num<<" last PageNum is "<<msg.last_page_num);

	map<PageCommMsg, int>::iterator count_it;

	if (msg.is_writer)
	{
		count_it = fileWriterCounts.find(msg);
		if (count_it == fileWriterCounts.end())
		{
			OPTIONHEDGE_LOG_ERROR(logger, "can not find key at fileWriterCounts in exit client");
			return;
		}
	}
	else
	{
		count_it = fileReaderCounts.find(msg);
		if (count_it == fileReaderCounts.end())
		{
			OPTIONHEDGE_LOG_ERROR(logger, "can not find key at fileReaderCounts in exit client");
			return;
		}
	}

	count_it->second--;
	if (count_it->second == 0)
	{
		bool othersideEmpty = 0;
		if (msg.is_writer)
		{
			fileWriterCounts.erase(count_it);
			othersideEmpty = fileReaderCounts.find(msg) == fileReaderCounts.end();
		}
		else
		{
			fileReaderCounts.erase(count_it);
			othersideEmpty = fileWriterCounts.find(msg) == fileWriterCounts.end();
		}

		//do the release for page if no writer and no reader
		if (othersideEmpty)
		{
			string path = PageUtil::GenPageFullPath(msg.name, msg.folder, msg.page_num);
			auto file_it = fileAddrs.find(path);
			if (file_it != fileAddrs.end())
			{
				void *addr = file_it->second;
				OPTIONHEDGE_LOG_ERROR(logger, "[AddrRm] (path)" << path << " (addr)" << addr);
				PageUtil::ReleasePageBuffer(addr, JOURNAL_PAGE_SIZE, true);
				fileAddrs.erase(file_it);
			}
		}
	}
}

byte PageEngine::initiate_page(const PageCommMsg& msg)
{
	OPTIONHEDGE_LOG_INFO(logger, "[InPage] (folder)" << msg.folder << " (jname)" << msg.name << " (pNum)" << msg.page_num << " (lpNum)" << msg.last_page_num << " (writer?)" << msg.is_writer);

	string path = PageUtil::GenPageFullPath(msg.folder, msg.name, msg.page_num);
	if (fileAddrs.find(path) == fileAddrs.end())
	{
		void* buffer = nullptr;
		if (!PageUtil::FileExists(path))
		{
			if (!msg.is_writer)
			{
				return PAGED_COMM_NON_EXIST;
			}
			else
			{
				const string tempPath = TEMP_PAGE;
				auto tempPageIter = fileAddrs.find(tempPath);
				if (tempPageIter != fileAddrs.end())
				{
					int ret = rename(tempPath.c_str(), path.c_str());
					if (ret < 0)
					{
						OPTIONHEDGE_LOG_ERROR(logger, "[InPage] ERROR: Cannot rename from " << tempPath << " to " << path);
						return PAGED_COMM_CANNOT_RENAME_FROM_TEMP;
					}
					else
					{
						OPTIONHEDGE_LOG_INFO(logger, "[InPage] TEMP_POOL: " << tempPath << " to " << path);
						buffer = tempPageIter->second;
						fileAddrs.erase(tempPageIter);
					}
				}
				if (buffer == nullptr)
				{
					buffer = PageUtil::LoadPageBuffer(path, JOURNAL_PAGE_SIZE, true, true);
				}
			}
		}
		else
		{
			buffer = PageUtil::LoadPageBuffer(path, JOURNAL_PAGE_SIZE, false, true);
		}

		OPTIONHEDGE_LOG_INFO(logger, "[AddrAdd] (path)" << path << " (addr)" << buffer);
		fileAddrs[path] = buffer;
	}

	if (msg.is_writer)
	{
		auto count_it = fileWriterCounts.find(msg);
		if (count_it == fileWriterCounts.end())
		{
			fileWriterCounts[msg] = 1;
		}
		else
		{
			return PAGED_COMM_MORE_THAN_ONE_WRITE;
		}
	}
	else
	{
		auto count_it = fileReaderCounts.find(msg);
		if (count_it == fileReaderCounts.end())
		{
			fileReaderCounts[msg] = 1;
		}
		else
		{
			fileReaderCounts[msg]++;
		}
	}
	return PAGED_COMM_ALLOCATED;
}

void PageEngine::exit_client(const string& clientName)
{
	map<string, PageClientInfo>::iterator it = clientJournals.find(clientName);

	if (it == clientJournals.end())
		return;

	PageClientInfo& info = it->second;

	if (info.is_strategy)
	{
		int idx = info.user_index_vec[0];
		PageCommMsg* msg = GET_COMM_MSG(commBuffer, idx);
		json j_request;
		j_request["name"] = msg->name;
		j_request["folder"] = msg->folder;
		j_request["rid_s"] = info.rid_start;
		j_request["rid_e"] = info.rid_end;
		j_request["pid"] = info.pid;
//		j_request["last_switch_nano"] = last_switch_nano;
		write(j_request.dump(), MSG_TYPE_STRATEGY_END);
	}

	for (auto idx : info.user_index_vec)
	{
		PageCommMsg* msg = GET_COMM_MSG(commBuffer, idx);
		if (msg->status == PAGED_COMM_ALLOCATED)
		{
			release_page(*msg);
		}
		msg->status = PAGED_COMM_RAW;
	}
	OPTIONHEDGE_LOG_INFO(logger, "[RmClient] (name)" << clientName << " (start)" << info.reg_nano << " (end)" << getNanoTime());

	vector<string>& clients = pidClient[info.pid];
	clients.erase(remove(clients.begin(), clients.end(), clientName), clients.end());
	if (clients.size() == 0)
	{
		pidClient.erase(info.pid);
	}
	clientJournals.erase(it);
}

void PageEngine::start_comm()
{
	comm_running = true;
	for (size_t idx = 0; comm_running; idx = (idx + 1) % (maxIdx + 1))
	{
		PageCommMsg* msg = GET_COMM_MSG(commBuffer, idx);
		if (msg->status == PAGED_COMM_REQUESTING)
		{
			acquire_mutex();
			OPTIONHEDGE_LOG_INFO(logger, "[Demand idx] " << idx);
			if (msg->last_page_num > 0 && msg->last_page_num != msg->page_num)
			{
				short curPage = msg->page_num;
				msg->page_num = msg->last_page_num;
				release_page(*msg);
				msg->page_num = curPage;
			}
			msg->status = initiate_page(*msg);
			msg->last_page_num = msg->page_num;
			release_mutex();
		}
	}
}

int PageEngine::getRequestidIndex(string name,const std::string& configFile, short source)
{
	std::ifstream ifs(configFile);
	json j_config = json::parse(ifs);
	if (source == SOURCE_CTP)
	{
		auto iter = j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].find("StrategyRange");
		if (iter != j_config[TE_CONFIG_KEY_TD][TE_CONFIG_KEY_SOURCECTP].end())
		{
			for (auto& j_strategyRange : iter.value())
			{
				if (j_strategyRange["name"].get<string>() == name)
				{
					return j_strategyRange["rid_start"].get<int>();
				}
			}
		}
		throw std::runtime_error("StrategyRange do not config,can not register strategy");
	}
}

IntPair PageEngine::register_strategy(const string& strategyName)
{
	map<string, PageClientInfo>::iterator it = clientJournals.find(strategyName);
	if (it == clientJournals.end())
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[ERROR] cannot find client " << strategyName << " in register_strategy");
		return std::make_pair(-1, -1);
	}

	PageClientInfo& info = it->second;
	int idx = info.user_index_vec[0]; // strategy must be a writer, therefore only one user
	info.is_strategy = true;
	// make sure this rid start from REQUEST_ID_RANGE,
	// [0, REQUEST_ID_RANGE - 1] belongs to trade engine themselves.
	int startRid=getRequestidIndex(strategyName, CONFIGURATION_JSON_FILE, SOURCE_INDEX);
	
	info.rid_start = startRid;
	info.rid_end = startRid + REQUEST_ID_RANGE - 1;

	PageCommMsg* msg = GET_COMM_MSG(commBuffer, idx);
	json j_request;
	j_request["name"] = strategyName;
	j_request["folder"] = msg->folder;
	j_request["rid_s"] = info.rid_start;
	j_request["rid_e"] = info.rid_end;
	j_request["pid"] = info.pid;
//	j_request["last_switch_nano"] = last_switch_nano;
	write(j_request.dump(), MSG_TYPE_STRATEGY_START);
	OPTIONHEDGE_LOG_INFO(logger, "[RegStrategy] (name)" << strategyName << " (rid)" << info.rid_start << "-" << info.rid_end);
	return std::make_pair(info.rid_start, info.rid_end);
}

bool PageEngine::sub_md(const vector<string>& tickers, short source, short msgType, bool isLast)
{
	OPTIONHEDGE_LOG_INFO(logger, "(subscribe) (source)" << source << " (msg)" << msgType << " (num)" << tickers.size() << " (last)" << isLast);
	bool written = true;
	for (size_t i = 0; i < tickers.size(); i++)
		written &= write(tickers[i], msgType, isLast && (i == tickers.size() - 1), source);
	return written;
}

bool PageEngine::login_td(const string& clientName, short source)
{
	OPTIONHEDGE_LOG_INFO(logger, "[TELogin] (name)" << clientName << " (source)" << source);
	map<string, PageClientInfo>::iterator it = clientJournals.find(clientName);
	if (it == clientJournals.end())
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[ERROR][TELogin] this client does not exist! (name)" << clientName);
		return false;
	}
	PageClientInfo& info = it->second;
	if (info.user_index_vec.size() != 1)
	{
		OPTIONHEDGE_LOG_ERROR(logger, "[ERROR][TELogin] this client suppose to have only one journal! (name)" << clientName);
		return false;
	}

	PageCommMsg* msg=GET_COMM_MSG(commBuffer, info.user_index_vec[0]);

	json j_request;
	j_request["name"] = clientName;
	j_request["folder"] = msg->folder;
	j_request["rid_s"] = info.rid_start;
	j_request["rid_e"] = info.rid_end;
	j_request["pid"] = info.pid;
//	j_request["last_switch_nano"] = last_switch_nano;
	write(j_request.dump(), MSG_TYPE_TRADE_ENGINE_LOGIN, true, source);
	info.trade_engine_vec.push_back(source);
	return true;
}

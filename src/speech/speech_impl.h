#pragma once

#include <mutex>
#include <condition_variable>
#include <list>
#include <string>
#include <memory>
#include <thread>
#include "speech.h"
#include "types.h"
#include "speech.pb.h"
#include "op_ctl.h"
#include "pending_queue.h"
#include "speech_connection.h"

namespace rokid {
namespace speech {

typedef OperationController<SpeechStatus, SpeechError> SpeechOperationController;

class SpeechImpl : public Speech {
public:
	SpeechImpl();

	bool prepare();

	void release();

	int32_t put_text(const char* text);

	int32_t start_voice();

	void put_voice(int32_t id, const uint8_t* data, uint32_t length);

	void end_voice(int32_t id);

	void cancel(int32_t id);

	bool poll(SpeechResult& res);

	void config(const char* key, const char* value);

private:
	inline int32_t next_id() { return ++next_id_; }

	void send_reqs();

	void gen_results();

	void gen_result_by_resp(rokid::open::SpeechResponse& resp);

	bool gen_result_by_status();

	int32_t do_request(std::shared_ptr<SpeechReqInfo>& req);

	bool do_ctl_change_op(std::shared_ptr<SpeechReqInfo>& req);

private:
	int32_t next_id_;
	SpeechConfig config_;
	SpeechConnection connection_;
	std::list<std::shared_ptr<SpeechReqInfo> > text_reqs_;
	StreamQueue<std::string> voice_reqs_;
	StreamQueue<SpeechResultIn> responses_;
	std::mutex req_mutex_;
	std::condition_variable req_cond_;
	std::mutex resp_mutex_;
	std::condition_variable resp_cond_;
	SpeechOperationController controller_;
	std::thread* req_thread_;
	std::thread* resp_thread_;
	bool initialized_;
};

} // namespace speech
} // namespace rokid

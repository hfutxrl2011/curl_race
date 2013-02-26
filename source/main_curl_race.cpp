
#include <curl/curl.h>
#ifdef min
#undef min
#endif

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {

	void logError(const std::string& msg) { std::cerr << msg << std::endl; }

	bool curl_init() {
		static bool once = false;
		if (!once) {
			CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
			if (res != CURLE_OK) {
				logError("curl_global_init");
				logError(curl_easy_strerror(res));
			} else {
				once = true;
			}
		}
		return once;
	}

	void curl_cleanup() {
		curl_global_cleanup();
	}

	template<typename T>
		bool setopt(CURL* curl, CURLoption opt, const T& val) {
			CURLcode res = curl_easy_setopt(curl, opt, val);
			if (res != CURLE_OK) {
				logError("curl_easy_setopt");
				logError(curl_easy_strerror(res));
				return false;
			}
			return true;
		}


	bool perform(CURL* curl) {
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			//* // too noisy
			logError("curl_easy_perform");
			logError(curl_easy_strerror(res));
			// */
			return false;
		}
		return true;
	}

	struct ReadState {
		ReadState()
			: offset(0), remains(0) {}
		ReadState(char* offset, size_t remains)
			: offset(offset), remains(remains) {}
		char* offset;
		size_t remains;
	};

	size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
	{
		ReadState* state = static_cast<ReadState*>(userp);

		size_t put_sz = std::min(size*nmemb, state->remains);

		if(put_sz) {
			char* put_ptr = static_cast<char*>(ptr);

			for (size_t i = 0; i < put_sz; i++)
				put_ptr[i] = state->offset[i];

			state->offset += put_sz;
			state->remains -= put_sz;
		}

		return put_sz;
	}

	size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
		return size*nmemb;
	}

} // anon

int main(int argc, char* argv[]) {
	std::vector<std::string> args(argv, argv+argc);

	std::string url;
	size_t iterations = 1000;
	try {
		url = args.at(1);
		if (args.size() > 2)
			iterations = atoi(args.at(2).c_str());
	} catch (std::exception& e) {
		std::cerr << "exception: " << e.what() << std::endl;
		std::cerr << "usage: " << args.at(0) << " url iterations" << std::endl;
		return 1;
	}

	curl_init();

	CURL* curl = curl_easy_init();
	if (!curl) {
		logError("curl_easy_init");
		return 1;
	}

	setopt(curl, CURLOPT_URL, url.c_str());
	setopt(curl, CURLOPT_TIMEOUT, 5L);
	setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
	setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	setopt(curl, CURLOPT_HEADER, 1L);
	setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);

	char send_data[] = "words, words, words";

	for (size_t i = 0; i < iterations; i++) {
		ReadState read_state(send_data, strlen(send_data));
		setopt(curl, CURLOPT_POST, 1L);
		setopt(curl, CURLOPT_READFUNCTION, read_callback);
		setopt(curl, CURLOPT_READDATA, &read_state);
		setopt(curl, CURLOPT_POSTFIELDSIZE, read_state.remains);

		bool sent = false;
		for (size_t i = 0; i < 3; i++) {
			if (perform(curl)) {
				std::cout << "^";
				sent = true;
				break;
			}
			std::cout << ".";
		}
		if (!sent) {
			std::cout << "!";
		}
	}

	curl_easy_cleanup(curl);
	curl_cleanup();

	return 0;
}

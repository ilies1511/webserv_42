#include "../../Includes/request_parser/RequestParser.hpp"
#include "parser_internal.hpp"
#include <cassert>
#include <unistd.h>
#include <fcntl.h>


const char *dummy_input =
"POST http://example.com:443/test/path/file.txt HTTP/1.1\r\n" "Host: www.example.re\r\n"
"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)\r\n"
"Accept: text/html\r\n"
"Accept-Language: en-US, en; q=0.5\r\n"
"Accept-Encoding: gzip, deflate\r\n"
//"content-length:   10\r\n"
"Transfer-Encoding: chunked\r\n"
"\r\n"
"0x5\r\n"
"Hello\r\n"
"0x1\r\n"
" \r\n"
"0x6\r\n"
"World!\r\n"
"0x0\r\n"
"0xff\r\nnew message shouldn't be parsed\r\n"
;

class Tests {
public:
	size_t			passes;
	size_t			fails;

	std::vector<std::string>		method_strs;
	std::vector<std::string>		methods;
	std::vector<std::optional<int>>	method_codes;

	std::vector<std::string>		uri_strs;
	std::vector<Uri>				uris;
	std::vector<std::optional<int>>	uri_codes;

	std::vector<std::string>		version_strs;
	std::vector<std::string>		versions;
	std::vector<std::optional<int>>	version_codes;

	Tests():
		passes(0),
		fails(0),
		parser(nullptr)
	{
		init_methods();
		init_uris();
		init_versions();
		assert_sizes();
	}

	// currently ignores that the parser has no early error detecting if the request line isn't finished
	void run_combination_tests() {
		assert_sizes();
		std::string before_method_str = "";
		Request before_method = Request();
		before_method.body = "";
		for (size_t m = 0; m < methods.size(); m++) {
			//expect.body = "";
			request_str = before_method_str;
			expect = before_method;
			comb_method(m);
			request_str += " ";
			//if (expect.status_code.has_value()) {
			//	run_test();
			//	continue ;
			//}
			std::string before_uri_str = request_str;
			Request before_uri = expect;
			for (size_t u = 0; u < uris.size(); u++) {
				request_str = before_uri_str;
				expect = before_uri;
				comb_uri(u);
				request_str += " ";
				//if (expect.status_code.has_value()) {
				//	run_test();
				//	continue ;
				//}
				std::string before_version_str = request_str;
				Request before_version = expect;
				for (size_t v = 0; v < versions.size(); v++) {
					expect = before_version;
					request_str = before_version_str;
					comb_version(v);
					request_str += "\r\n";
					if (expect.status_code.has_value()) {
						run_test();
						continue ;
					}
					for (int i = 0; i < 1; i++ /*todo: headers later */) {
						for (int i = 0; i < 1; i++ /*todo: bodys later */) {
							run_test();
						}
					}
				}
			}
		}
		std::cout << "passes: " << passes << "; fails: " << fails << "!" << std::endl;
	}

	bool run_whitespace_tests() {
		//todo: later
		return (true);
	}

private:
	std::string		request_str;

	Request			expect;
	Request			actual;
	RequestParser	*parser;

	void run_test(void) {
		//std::cout << std::flush;
		//int tmp = open("/dev/null", O_WRONLY);
		//int std_out = dup(1);
		//if (std_out < 0 || dup2(tmp, 1) < 0 || close(tmp) < 0) {
		//	std::cerr << strerror(errno) << std::endl;
		//	assert(errno == 0);
		//}

		parser = new RequestParser(request_str);
		if (parser->parse_request_line()) {
			if (parser->parse_headers()) {
				if (parser->parse_body(DEFAULT_MAX_REQ_BODY_SIZE)) {
				}
			}
		}
		actual = parser->getRequest();
		//std::cout << std::flush;
		//if (dup2(std_out, 1) < 0 || close(std_out) < 0) {
		//	std::cerr << strerror(errno) << std::endl;
		//	assert(errno == 0); //this asserts
		//}
		//std::cout << std::flush;
		if (actual == expect) {
			//std::cout << "test case |" << request_str << "|passed" << std::endl;
			passes++;
			//std::cout << "##############expected:\n" << expect << "#############got:\n" << actual;
			std::cout << "**********************\n";
		} else {
			std::cout << "test case |" << request_str << "|failed" << std::endl;
			std::cout << "##############expected:\n" << expect << "#############got:\n" << actual;

			std::cout << "*******************************************\n";
			fails++;
		}
		delete parser;
		parser = nullptr;
	}

	void comb_method(size_t idx) {
		request_str += method_strs[idx];
		if (!expect.status_code.has_value()) {
			expect.status_code = method_codes[idx];
		}
		if (!expect.status_code.has_value()) {
			expect.method = methods[idx];
		}
	}

	void comb_uri(size_t idx) {
		request_str += uri_strs[idx];
		if (!expect.status_code.has_value()) {
			expect.status_code = uri_codes[idx];
		}
		if (!expect.status_code.has_value()) {
			expect.uri = uris[idx];
		}
	}

	void comb_version(size_t idx) {
		request_str += version_strs[idx];
		if (!expect.status_code.has_value()) {
			expect.status_code = version_codes[idx];
		}
		if (!expect.status_code.has_value()) {
			expect.version = versions[idx];
		}
	}

	void assert_sizes() {

		assert(methods.size());
		assert(method_strs.size() == methods.size());
		assert(method_codes.size() == methods.size());

		assert(uris.size());
		assert(uri_strs.size() == uris.size());
		assert(uri_codes.size() == uris.size());

		assert(versions.size());
		assert(version_strs.size() == versions.size());
		assert(version_codes.size() == versions.size());
	}

	void init_methods(void) {
		methods.push_back("GET");
		method_strs.push_back("GET");
		method_codes.push_back(std::nullopt);

		methods.push_back("get");
		method_strs.push_back("get");
		method_codes.push_back(501);
	
		methods.push_back("GETT");
		method_strs.push_back("GETT");
		method_codes.push_back(501);
	
		methods.push_back("POST");
		method_strs.push_back("POST");
		method_codes.push_back(std::nullopt);
	
		methods.push_back("DELETE");
		method_strs.push_back("DELETE");
		method_codes.push_back(std::nullopt);
	
		methods.push_back(" DELETE");
		method_strs.push_back("");
		method_codes.push_back(400);

		methods.push_back("DELETE ");
		method_strs.push_back("");
		method_codes.push_back(400);

		methods.push_back("\tGET");
		method_strs.push_back("");
		method_codes.push_back(400);
	}

	void init_uris(void) {
		{
			Uri uri;
			std::string str = "/search?q=test";
			uri.full = str;
			uri.path = "/search";
			uri.query = "q=test";
			uri.form.is_origin_form = 1;

			uris.push_back(uri);
			uri_strs.push_back(str);
			uri_codes.push_back(std::nullopt);
		}
	}

	void init_versions(void) {
		versions.push_back("HTTP/1.1");
		version_strs.push_back("HTTP/1.1");
		version_codes.push_back(std::nullopt);

		versions.push_back("HTTP/1.0");
		version_strs.push_back("HTTP/1.0");
		version_codes.push_back(505);

		versions.push_back("http/1.1");
		version_strs.push_back("http/1.1");
		version_codes.push_back(505);

		versions.push_back("HTTP/1.2");
		version_strs.push_back("HTTP/1.2");
		version_codes.push_back(505);

		versions.push_back("HTTP/1.11");
		version_strs.push_back("HTTP/1.11");
		version_codes.push_back(505);

		versions.push_back("HTTP1.1");
		version_strs.push_back("HTTP1.1");
		version_codes.push_back(400);

		versions.push_back("HTTP/1.");
		version_strs.push_back("HTTP/1.");
		version_codes.push_back(400);

		versions.push_back("HTTP/11");
		version_strs.push_back("HTTP/11");
		version_codes.push_back(400);

		versions.push_back("HTTP/1 1");
		version_strs.push_back("HTTP/1 1");
		version_codes.push_back(400);

		versions.push_back("HTTP/.1");
		version_strs.push_back("HTTP/.1");
		version_codes.push_back(400);

		versions.push_back("HTTP-1.1");
		version_strs.push_back("HTTP-1.1");
		version_codes.push_back(400);

		versions.push_back("HTTP/1\\1");
		version_strs.push_back("HTTP-1.1");
		version_codes.push_back(400);
	}
};

int main(void) {
	Tests tester;

	errno = 0;
	tester.run_combination_tests();
}

#include "../../Includes/request_parser/RequestParser.hpp"
#include <cassert>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

class Tests {
public:
	size_t			passes;
	size_t			fails;

	size_t			max_body_len;

	std::vector<std::string>				method_strs;
	std::vector<std::string>				methods;
	std::vector<std::optional<int>>			method_codes;

	std::vector<std::string>				uri_strs;
	std::vector<Uri>						uris;
	std::vector<std::optional<int>>			uri_codes;

	std::vector<std::string>				version_strs;
	std::vector<std::string>				versions;
	std::vector<std::optional<int>>			version_codes;


	struct header {
		std::optional<int>	status_code = std::nullopt;
		/* Headers vector:
		 * [0]: name
		 * [1]: seperator
		 * [2]: value
		 * [3]: termination
		*/
		std::vector<std::vector<std::string>>			headers;
		std::unordered_map<std::string, std::string>	expect;
	};
	std::vector<struct header>	headers;

	struct body {
		std::vector<std::pair<std::string, std::string>>		headers;
		std::string				body_input;
		std::string				expected_body;
		std::optional<int>		status_code;
		size_t					max_body_len = DEFAULT_MAX_REQ_BODY_SIZE;
	};

	std::vector<struct body>		bodies;

	Tests():
		passes(0),
		fails(0),
		parser(nullptr)
	{
		init_methods();
		init_uris();
		init_versions();
		init_headers();
		init_bodies();
		assert_sizes();
	}

	// todo:
	// make hardcoded whitespace in this function part of the testcases
	// body, content-length, chunked-encoding
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
			std::string before_uri_str = request_str;
			Request before_uri = expect;
			for (size_t u = 0; u < uris.size(); u++) {
				request_str = before_uri_str;
				expect = before_uri;
				comb_uri(u);
				request_str += " ";
				std::string before_version_str = request_str;
				Request before_version = expect;
				for (size_t v = 0; v < versions.size(); v++) {
					expect = before_version;
					request_str = before_version_str;
					comb_version(v);
					request_str += "\r\n";
					// early test run to reduce run time, comment for full test
					if (expect.status_code.has_value()) {
						run_test();
						continue ;
					}
					std::string before_headers_str = request_str;
					Request before_headers = expect;
					for (size_t h = 0; h < headers.size(); h++) {
						expect = before_headers;
						request_str = before_headers_str;
						comb_headers(h);
						std::string before_body_str = request_str;
						Request before_body = expect;
						//request_str += "\r\n";
						for (size_t b = 0; b < bodies.size(); b++) {
							request_str = before_body_str;
							expect = before_body;
							comb_body(b);
							run_test();
						}
					}
				}
			}
		}
		std::cout << "passes: " << passes << "; fails: " << fails << "!" << std::endl;
	}

private:
	std::string		request_str;

	Request			expect;
	Request			actual;
	RequestParser	*parser;

	void run_test(void) {
		std::cout << std::flush;
		int tmp = open("log", O_WRONLY | O_TRUNC | O_CREAT, 0644);
		int std_out = dup(1);
		if (std_out < 0 || dup2(tmp, 1) < 0 || close(tmp) < 0) {
			std::cerr << strerror(errno) << std::endl;
			assert(errno == 0);
		}

		//std::cerr << request_str <<std::endl;
		parser = new RequestParser(request_str);
		if (parser->parse_request_line()) {
			if (parser->parse_headers()) {
				if (parser->parse_body(this->max_body_len)) {
				}
			}
		}
		actual = parser->getRequest();
		std::cout << std::flush;
		if (dup2(std_out, 1) < 0 || close(std_out) < 0) {
			std::cerr << strerror(errno) << std::endl;
			assert(errno == 0); //this asserts
		}
		std::cout << std::flush;
		if (actual == expect) {
			//std::cout << "test case |" << request_str << "|passed" << std::endl;
			passes++;
			//std::cout << "##############expected:\n" << expect << "#############got:\n" << actual;
			//std::cout << "********<**************\n";
		} else {
			std::cout << "test case " <<  passes + fails << ": |" << request_str << "|failed" << std::endl;
			std::cout << "max_body_len: " << this->max_body_len << "\n";
			std::cout << "##############expected:\n" << expect << "#############got:\n" << actual;
			std::cout << "#######\n";
			std::cout << "parser logging: \n";
			std::ifstream file("log", std::ios::binary | std::ios::ate);
			assert(file.is_open());
			char buf[1000];
			long size = file.tellg();
			size = size > static_cast<long>(sizeof buf) - 1 ? sizeof buf - 1 : size;
			buf[size] = 0;
			file.seekg(0, std::ios::beg);
			file.read(buf, size);
			std::cout << buf;
			file.close();
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
		expect.uri = uris[idx];
		if (!expect.status_code.has_value()) {
			expect.status_code = uri_codes[idx];
		}
	}

	void comb_version(size_t idx) {
		request_str += version_strs[idx];
		if (expect.status_code.has_value()) {
			return ;
		}
		expect.status_code = version_codes[idx];
		if (expect.status_code.has_value()) {
			return ;
		}
		expect.version = versions[idx];
	}

	void comb_headers(size_t idx) {

		struct header &headers = this->headers[idx];

		for (const auto &line : headers.headers) {
			request_str += line[0];
			request_str += line[1];
			request_str += line[2];
			request_str += line[3];
		}
		if (this->expect.status_code.has_value()) {
			return ;
		}
		this->expect.headers = headers.expect;
		this->expect.status_code = headers.status_code;
	}

	void comb_body(size_t idx) {
		struct body body;
		for (size_t i = 0; i < bodies[idx].headers.size(); i++) {
			this->request_str += bodies[idx].headers[i].first + ": " + bodies[idx].headers[i].second + "\r\n";
			this->expect.headers[bodies[idx].headers[i].first] = bodies[idx].headers[i].second;
		}
		this->max_body_len = bodies[idx].max_body_len;
		this->request_str += "\r\n";
		this->request_str += bodies[idx].body_input;
		this->expect.body = bodies[idx].expected_body;
		if (!this->expect.status_code.has_value()) {
			this->expect.status_code = bodies[idx].status_code;
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

		methods.push_back("GET");
		method_strs.push_back("GET   ");
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
			std::string str = "/search?q%3Dtest";
			uri.full = str;
			uri.path = "/search";
			uri.query = "q=test";
			uri.form.is_origin_form = 1;

			uris.push_back(uri);
			uri_strs.push_back(str);
			uri_codes.push_back(std::nullopt);
		}
		{
			Uri uri;
			uri.full = "/";
			uri.path = "/";
			uri.form.is_origin_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(std::nullopt);
		}
		{
			Uri uri;
			uri.full = "/path/with%20space";
			uri.path = "/path/with space"; // Parser should preserve encoding
			uri.form.is_origin_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(std::nullopt);
		}
		{
			Uri uri;
			uri.full = "?query%20emptypath";
			uri.path = "/"; // should normalize empty path to "/" ?
			uri.query = "query=empty-path";
			uri.form.is_origin_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(400);
		}
		// 2. VALID ABSOLUTE FORM (RFC 7230 §5.3.2)
		{
			Uri uri;
			uri.full = "http://localhost:8080/path?query";
			uri.authority = "localhost:8080";
			uri.host = "localhost";
			uri.port = "8080";
			uri.path = "/path";
			uri.query = "query";
			uri.form.is_absolute_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(std::nullopt);
		}
		// https request
		{
			Uri uri;
			uri.full = "https://www.example.com/";
			uri.authority = "www.example.com";
			uri.host = "www.example.com";
			uri.path = "/";
			uri.form.is_absolute_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(400);
		}
		
		{
		//todo: Ipv6 parser support
			Uri uri;  // IPv6 address
			uri.full = "http://[2001:db8::1]:8080/path";
			uri.authority = "[2001:db8::1]:8080";
			uri.host = "2001:db8::1";
			uri.port = "8080";
			uri.path = "/path";
			uri.form.is_absolute_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(std::nullopt);
		}

		// 3. VALID AUTHORITY FORM (RFC 7230 §5.3.3)
		{
			Uri uri;
			uri.full = "localhost:8080";
			uri.authority = "localhost:8080";
			uri.host = "localhost";
			uri.port = "8080";
			uri.form.is_authority_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(std::nullopt);
		}
		{
		//todo: IPv6 parser support
			Uri uri;  // IPv6 authority
			uri.full = "[::1]:443";
			uri.authority = "[::1]:443";
			uri.host = "::1";
			uri.port = "443";
			uri.form.is_authority_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(std::nullopt);
		}

		// 4. VALID ASTERISK FORM (RFC 7230 §5.3.4)
		// not implemented
		{
			Uri uri;
			uri.full = "*";
			uri.path = "*";
			uri.form.is_asterisk_form = 1;
			uris.push_back(uri);
			uri_strs.push_back(uri.full);
			uri_codes.push_back(501);//does not support OPTIONS method
		}

		// 5. INVALID URIS
		{
			// Mixed form
			uri_strs.push_back("http://host/path*");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Invalid absolute URI
			uri_strs.push_back("http:///missing-host");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Invalid port
			uri_strs.push_back("host:invalid-port");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Space in URI
			uri_strs.push_back("/path with space");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Invalid IPv6
			// todo: IPv6 parser support
			uri_strs.push_back("http://[::1/");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Multiple query markers
			uri_strs.push_back("/path?query?invalid");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Invalid asterisk form
			uri_strs.push_back("*invalid");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Missing authority
			uri_strs.push_back("http://");
			uris.push_back(Uri());
			uri_codes.push_back(400);
		}
		{
			// Invalid percent encoding
			uri_strs.push_back("/path%xx");
			uris.push_back(Uri());
			uri_codes.push_back(400);
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

		versions.push_back("HTTP/1.11");
		version_strs.push_back("HTTP/1.11");
		version_codes.push_back(505);

		//versions.push_back("HTTP1.1");
		//version_strs.push_back("HTTP1.1");
		//version_codes.push_back(400);

		//versions.push_back("HTTP/1.");
		//version_strs.push_back("HTTP/1.");
		//version_codes.push_back(400);

		//versions.push_back("HTTP/11");
		//version_strs.push_back("HTTP/11");
		//version_codes.push_back(400);

		//versions.push_back("HTTP/1 1");
		//version_strs.push_back("HTTP/1 1");
		//version_codes.push_back(400);

		//versions.push_back("HTTP/.1");
		//version_strs.push_back("HTTP/.1");
		//version_codes.push_back(400);

		//versions.push_back("HTTP-1.1");
		//version_strs.push_back("HTTP-1.1");
		//version_codes.push_back(400);

	}

	void init_headers(void) {
		{
			struct header header;

			this->headers.push_back(header);
		}

		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "SomE Header1";
			comp[1] = ": ";
			comp[2] = "SOmE Key1";
			comp[3] = "\r\n";
			comps.push_back(comp);

			expect["some header1"] = "SOmE Key1";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "SomE Header2";
			comp[1] = ":";
			comp[2] = "SOmE Key2";
			comp[3] = "\r\n";
			comps.push_back(comp);

			expect["some header2"] = "SOmE Key2";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "SomE Header3";
			comp[1] = ":";
			comp[2] = "SOmE Key3";
			comp[3] = "\r\n";
			comps.push_back(comp);

			expect["some header3"] = "SOmE Key3";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "shouldnt matter sice next header is invalid";
			comp[1] = ":";
			comp[2] = "shouldnt matter sice next header is invalid";
			comp[3] = "\r\n";
			comps.push_back(comp);

			comp[0] = "dosnt matter invalid seperator";
			comp[1] = ":\r";
			comp[2] = "dosnt matter, invalid sep";
			comp[3] = "\r\n";
			comps.push_back(comp);

			header.status_code = 400;
			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "shouldnt matter sice next header is invalid";
			comp[1] = ":";
			comp[2] = "shouldnt matter sice next header is invalid";
			comp[3] = "\r\n";
			comps.push_back(comp);

			comp[0] = "dosnt matter invalid seperator2";
			comp[1] = ":\n";
			comp[2] = "dosnt matter, invalid sep2";
			comp[3] = "\r\n";
			comps.push_back(comp);

			header.status_code = 400;
			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "sep with many whitespace";
			comp[1] = ":  \v\t\t ";
			comp[2] = "sep with many whitespace";
			comp[3] = "\r\n";
			comps.push_back(comp);

			expect["sep with many whitespace"] = "sep with many whitespace";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		{
			struct header header;
			std::vector<std::vector<std::string>>			comps;
			std::vector<std::string>						comp(4);
			std::unordered_map<std::string, std::string>	expect;

			comp[0] = "sep with 2 space";
			comp[1] = ":  ";
			comp[2] = "sep with 2 space";
			comp[3] = "\r\n";
			comps.push_back(comp);

			expect["sep with 2 space"] = "sep with 2 space";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
		// Test: Duplicate headers (should keep the last occurrence)
		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::vector<std::string> comp(4);
			std::unordered_map<std::string, std::string> expect;

			comp[0] = "Duplicate-Header";
			comp[1] = ": ";
			comp[2] = "FirstValue";
			comp[3] = "\r\n";
			comps.push_back(comp);

			// Reset comp for second occurrence.
			comp[0] = "duplicate-header";
			comp[1] = ": ";
			comp[2] = "SecondValue";
			comp[3] = "\r\n";
			comps.push_back(comp);

			// Assuming the parser overwrites duplicate headers.
			expect["duplicate-header"] = "SecondValue";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}

		// Test: Header with extra whitespace in name and value (expect trimming and lower-casing)
		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::vector<std::string> comp(4);
			std::unordered_map<std::string, std::string> expect;

			comp[0] = "Extra ending Space Header   ";
			comp[1] = ": ";
			comp[2] = "   Value with spaces   ";
			comp[3] = "\r\n";
			comps.push_back(comp);

			header.headers = comps;
			header.status_code = 400;
			this->headers.push_back(header);
		}
		// Test: Header with extra whitespace in name and value (expect trimming and lower-casing)
		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::vector<std::string> comp(4);
			std::unordered_map<std::string, std::string> expect;

			comp[0] = "   Extra beginning Space Header";
			comp[1] = ": ";
			comp[2] = "   Value with spaces   ";
			comp[3] = "\r\n";
			comps.push_back(comp);

			header.headers = comps;
			header.expect = expect;
			header.status_code = 400;
			this->headers.push_back(header);
		}

		// Test: Header with missing colon (invalid separator, should result in an error)
		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::vector<std::string> comp(4);

			comp[0] = "NoColonHeader";
			comp[1] = "";  // missing separator
			comp[2] = "NoColonValue";
			comp[3] = "\r\n";
			comps.push_back(comp);

			// Mark as an error.
			header.status_code = 400;
			header.headers = comps;
			this->headers.push_back(header);
		}

		// Test: Multiple valid header lines in a single block
		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::unordered_map<std::string, std::string> expect;

			{
				std::vector<std::string> comp(4);
				comp[0] = "Header-One";
				comp[1] = ": ";
				comp[2] = "Value1";
				comp[3] = "\r\n";
				comps.push_back(comp);
				expect["header-one"] = "Value1";
			}
			{
				std::vector<std::string> comp(4);
				comp[0] = "Header-Two";
				comp[1] = ": ";
				comp[2] = "Value2";
				comp[3] = "\r\n";
				comps.push_back(comp);
				expect["header-two"] = "Value2";
			}

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}

		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::vector<std::string> comp(4);
			std::unordered_map<std::string, std::string> expect;

			comp[0] = "Empty-Value";
			comp[1] = ": ";
			comp[2] = "";  // empty value
			comp[3] = "\r\n";
			comps.push_back(comp);
			expect["empty-value"] = "";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}

		// Test: Header with unusual but valid characters in the field name
		{
			struct header header;
			std::vector<std::vector<std::string>> comps;
			std::vector<std::string> comp(4);
			std::unordered_map<std::string, std::string> expect;

			comp[0] = "X-Custom_Header+Test";
			comp[1] = ": ";
			comp[2] = "SomeValue";
			comp[3] = "\r\n";
			comps.push_back(comp);
			expect["x-custom_header+test"] = "SomeValue";

			header.headers = comps;
			header.expect = expect;
			this->headers.push_back(header);
		}
	}

	void init_bodies() {
		{ 
			body test;
			test.headers = {{"content-length", "11"}};
			test.body_input = "Hello World";
			test.expected_body = "Hello World";
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"content-length", "11"}};
			test.body_input = "Hello World";
			test.max_body_len = 5;
			test.status_code = 413;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"content-length", "11"}};
			test.body_input = "Hello World";
			test.expected_body = "Hello World";
			test.max_body_len = 11;
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"content-length", "11"}};
			test.body_input = "Hello World";
			test.max_body_len = 10;
			test.status_code = 413;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"content-length", "5"}};
			test.body_input = "12345";
			test.expected_body = "12345";
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"content-length", "5"}};
			test.body_input = "123";
			test.expected_body = ""; // not finished body
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"content-length", "abc"}};
			test.body_input = "12345";
			test.expected_body = "";
			test.status_code = 400;
			bodies.push_back(test);
		}
	
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input =
				"5\r\n"
				"Hello\r\n"
				"6\r\n"
				" World\r\n"
				"0\r\n"
				"\r\n";
			test.expected_body = "Hello World";
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input =
				"5\r\n"
				"Hello\r\n"
				"6\r\n"
				" World\r\n"
				"0\r\n"
				"\r\n";
			test.expected_body = "Hello World";
			test.max_body_len = 11;
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input =
				"0\r\n"
				"\r\n";
			test.max_body_len = 10;
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input =
				"5\r\n"
				"Hello\r\n"
				"6\r\n"
				" World\r\n"
				"0\r\n"
				"\r\n";
			test.max_body_len = 10;
			test.status_code = 413;
			bodies.push_back(test);
		}
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input =
				"0\r\n"
				"\r\n";
			test.expected_body = "";
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
	
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input = "invalid_chunk_data";
			test.expected_body = "";
			test.status_code = 400;
			bodies.push_back(test);
		}
	
		{
			body test;
			test.headers.push_back({"content-length", "5"});
			test.headers.push_back({"transfer-encoding", "chunked"});
			test.body_input = "0\r\n\r\n";
			test.expected_body = "";
			test.status_code = 400;
			bodies.push_back(test);
		}
	
		{
			body test;
			test.headers = {{"transfer-encoding", "chunked"}};
			test.body_input =
				"5\r\n"
				"Hello\r\n"
				"0\r\n"
				"Trailer: X-Custom\r\n"
				"X-Custom: Value\r\n"
				"\r\n";
			test.expected_body = "Hello";
			test.status_code = std::nullopt;
			bodies.push_back(test);
		}
	}
};

int main(void) {
	Tests tester;

	tester.run_combination_tests();
}

#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include <typeinfo>

#include "my_global.h"
#include "mysql.h"

using namespace std;

static void print_stmt_error (MYSQL_STMT *stmt, const char *message) {
	fprintf (stderr, "%s\n", message);
	if (stmt != NULL) {
		fprintf(stderr, "Error %u (%s): %s\n",
			mysql_stmt_errno(stmt),
			mysql_stmt_sqlstate(stmt),
			mysql_stmt_error(stmt)); 
	} 
}

int main(int argc, char** argv) {
	bool embedded = false;
	if (argc > 1 && (strcmp(argv[1], "embedded") == 0)) {
		embedded = true;
	}

	if (embedded) {
		cout << "Hello you are connected in embedded mode" << endl;

		static const char *server_args[] = { "this_program", "--basedir=/usr/local/mysql", "--datadir=/Users/ravidesai/mysql/data", 
					     	"--plugin-dir=/Users/ravidesai/mysql/plugins", "--log-error=/Users/ravidesai/mysql/tmp/test.err",
					     	"--pid-file=/Users/ravidesai/mysql/tmp/test.pid",
					     	"--key_buffer_size=32M", "--log-bin=/Users/ravidesai/mysql/log/logbin"
					     	"--log-bin-trust-function-creators=1"
					     	"--log-bin-trust-routine-creators=1"
					    	};
		static const char *server_groups[] = { "embedded", "server", "this_program_SERVER", (char *) NULL };
	
		if (mysql_library_init(sizeof(server_args) / sizeof(char *), (char**) server_args, (char **)server_groups) != 0) {
			cout << "Error in Database::Initialize" << endl;
			return 1;
		}
	} else {
		cout << "Hello you are connected in standard client (non-embedded) mode" << endl;
		if (mysql_library_init(0, NULL, NULL) != 0) {
			cout << "Error in Database::Initialize" << endl;
		}
	}

	if (mysql_thread_init() != 0) {
		cout << "Error in Database::ThreadInitialize" << endl;
	}

	MYSQL *connect;
	connect = mysql_init(NULL);
	if (! connect) {
		cout << "MySQL initialization failure" << endl;
		return 1;
	}

	if (embedded) {
		mysql_options(connect, MYSQL_READ_DEFAULT_GROUP, "embedded");
		mysql_options(connect, MYSQL_OPT_USE_EMBEDDED_CONNECTION, NULL);
	}

	connect = mysql_real_connect(connect, "localhost", "root", "", "sakila", 0, NULL, 0);
	if (connect)
	{
		cout << "Connection succeeded" << endl;
	}	

	const char *stmt_str = "SELECT * FROM COUNTRY WHERE COUNTRY = ?";
	MYSQL_STMT *stmt;

	stmt = mysql_stmt_init(connect);

	if (mysql_stmt_prepare(stmt, stmt_str, strlen(stmt_str)) != 0) {
		print_stmt_error(stmt, "Count not prepare SELECT statement");
		return 1;
	}

	int numberParams = mysql_stmt_param_count(stmt);
	cout << "Number parameters: " << numberParams << endl;
	MYSQL_BIND param[numberParams];
	MYSQL_RES *metaData;
	if ((metaData = mysql_stmt_result_metadata(stmt)) == NULL) {
		print_stmt_error(stmt, "Error fetching stmt metadata");
	}

	int numberFields = 0;
	MYSQL_FIELD *field;
	while ((field = mysql_fetch_field(metaData)) != NULL) {
		numberFields++;
		cout << "[" << field->name << ", " << field->type <<  "] ";
	}
	cout << endl;

	const int STRING_SIZE = 50;

	int countryId;
	char countryName[STRING_SIZE];
	unsigned long countryNameLen;
	MYSQL_TIME lastUpdate;
	my_bool is_null[numberFields];

	MYSQL_BIND bind[numberFields];
	memset ((void *) &bind, 0, sizeof(bind));

	bind[0].buffer_type = MYSQL_TYPE_LONG;
	bind[0].buffer = (void *) &countryId;
	bind[0].is_unsigned = 0;
	bind[0].is_null = &is_null[0];

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = (void *) countryName;
	bind[1].is_null = &is_null[1];
	bind[1].buffer_length = sizeof(countryName);
	bind[1].length = &countryNameLen;

	bind[2].buffer_type = MYSQL_TYPE_DATETIME;
	bind[2].buffer = (void *) &lastUpdate;
	bind[2].is_null = &is_null[2];
	
	char thisCountry[STRING_SIZE];
	unsigned long thisCountryLen;

	param[0].buffer_type = MYSQL_TYPE_STRING;
	param[0].buffer = (char *)thisCountry;
	param[0].buffer_length = STRING_SIZE;
	param[0].is_null = 0;
	param[0].length = &thisCountryLen;

	cout << "binding parameters" << endl;
	if (mysql_stmt_bind_param(stmt, param) != 0) {
		print_stmt_error(stmt, "Could not bind SELECT params");
		return 1;
	}

	cout << "binding results" << endl;
	if (mysql_stmt_bind_result(stmt, bind) != 0) {
		print_stmt_error(stmt, "Count not bind SELECT results");
		return 1;
	}

	strncpy(thisCountry, "Canada", STRING_SIZE);
	thisCountryLen = strlen(thisCountry);

	cout << "executing statement" << endl;
	if (mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Could not execute SELECT statement");
		return 1;
	}

	cout << "storing results" << endl;
	if (mysql_stmt_store_result(stmt) != 0) {
		print_stmt_error(stmt, "Could not execute STORE RESULT");
		return 1;
	}

	cout << "fetching statement" << endl;
	while (mysql_stmt_fetch(stmt) == 0) {
		printf("[%d] ", countryId);
		printf("[%.*s] ", (int) countryNameLen, countryName);
		printf("[%04d-%02d-%02d %02d:%02d:%02d]", 
			lastUpdate.year, lastUpdate.month, lastUpdate.day,
                     	lastUpdate.hour, lastUpdate.minute, lastUpdate.second);
		printf("\n");
	}

	cout << "closing statement" << endl;
	mysql_stmt_close(stmt);
	cout << "closing database" << endl;
	mysql_close(connect);

	return 0;
}

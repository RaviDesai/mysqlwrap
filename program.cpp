#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include <typeinfo>

#include "my_global.h"
#include "mysql.h"

#include "Database.h"
#include "DatabaseException.h"
#include "Statement.h"
#include "ParamBuffer.h"

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
	cout << "Hello" << endl;

	MYSQL *connect;
	connect = mysql_init(NULL);
	if (! connect) {
		cout << "MySQL initialization failure" << endl;
		return 1;
	}

	connect = mysql_real_connect(connect, "localhost", "root", "", "sakila", 0, NULL, 0);
	if (connect)
	{
		cout << "Connection succeeded" << endl;
	}	

	MYSQL_RES *res_set;
	MYSQL_ROW row;

	mysql_query(connect,"SELECT * FROM COUNTRY"); 
	res_set = mysql_store_result(connect);

	unsigned int numrows = mysql_num_rows(res_set);
	cout << "Number rows: " << numrows << endl;

	unsigned int num_fields = mysql_num_fields(res_set);

	MYSQL_FIELD *field = NULL;

	while ((field = mysql_fetch_field(res_set)) != NULL) {
		cout << "[" << field->name << ", " << field->type <<  "] ";
	}
	cout << endl;

	while ((row = mysql_fetch_row(res_set)) != NULL) {
		unsigned long *lengths = mysql_fetch_lengths(res_set);
		for (unsigned int i = 0; i < num_fields; i++) {
			printf("[%.*s] ", (int) lengths[i], 
				row[i]  ? row[i] : "NULL");
		}
		printf("\n");
	}

	const char *stmt_str = "SELECT * FROM COUNTRY WHERE COUNTRY = ?";
	//int id;
	//char country[26];
	//MYSQL_TIME my_datetime;	
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

	if (mysql_stmt_bind_param(stmt, param) != 0) {
		print_stmt_error(stmt, "Could not bind SELECT params");
		return 1;
	}

	if (mysql_stmt_bind_result(stmt, bind) != 0) {
		print_stmt_error(stmt, "Count not bind SELECT results");
		return 1;
	}

	strncpy(thisCountry, "Canada", STRING_SIZE);
	thisCountryLen = strlen(thisCountry);

	if (mysql_stmt_execute(stmt) != 0) {
		print_stmt_error(stmt, "Could not execute SELECT statement");
		return 1;
	}

	if (mysql_stmt_store_result(stmt) != 0) {
		print_stmt_error(stmt, "Could not execute STORE RESULT");
		return 1;
	}

	while (mysql_stmt_fetch(stmt) == 0) {
		printf("[%d] ", countryId);
		printf("[%.*s] ", (int) countryNameLen, countryName);
		printf("[%04d-%02d-%02d %02d:%02d:%02d]", 
			lastUpdate.year, lastUpdate.month, lastUpdate.day,
                     	lastUpdate.hour, lastUpdate.minute, lastUpdate.second);
		printf("\n");
	}

	mysql_stmt_close(stmt);
	mysql_close(connect);

	cout << "===> start using objects <===" << endl;

	try
	{	
		Database db("localhost", "root", "", "Sakila", 0, NULL, 0);
		db.Connect();
		/*
		Statement stmt(db, "select * from country where country = ?");
		stmt.AssignNextParameter(Nullable<string>("USA"));
		stmt.Execute();
		stmt.FetchNextRow();
		Nullable<short int> countryId = stmt.GetShortDataInRow(0);
		Nullable<std::string> countryName = stmt.GetStringDataInRow(1);
		Nullable<MYSQL_TIME> lastUpdate = stmt.GetTimeDataInRow(2);

		cout << "Id: " << countryId << " Name: " << countryName << " Last Update: " 
		     << lastUpdate.year << "-" << lastUpdate.month << "-" << lastUpdate.day << " " 
		     << lastUpdate.hour << ":" << lastUpdate.minute << ":" << lastUpdate.second
		     << endl;

		stmt.ResetParameters();
		stmt.AssignNextParameter(Nullable<string>("Canada"));
		stmt.Execute();
		stmt.FetchNextRow();
		countryId = stmt.GetShortDataInRow(0);
		countryName = stmt.GetStringDataInRow(1);
		lastUpdate = stmt.GetTimeDataInRow(2);

		cout << "Id: " << countryId << " Name: " << countryName << " Last Update: " 
		     << lastUpdate.year << "-" << lastUpdate.month << "-" << lastUpdate.day << " " 
		     << lastUpdate.hour << ":" << lastUpdate.minute << ":" << lastUpdate.second
		     << endl;

		Statement insertStmt(db, "insert into country (country) values (?)");
		insertStmt.AssignNextParameter(Nullable<string>("India"));
		insertStmt.Execute();
		cout << "Inserted " << insertStmt.NumberOfAffectedRows() << " rows." << endl;

		stmt.ResetParameters();
		stmt.AssignNextParameter(Nullable<string>("India"));
		stmt.Execute();
		stmt.FetchNextRow();

		countryId = stmt.GetShortDataInRow(0);
		countryName = stmt.GetStringDataInRow(1);
		lastUpdate = stmt.GetTimeDataInRow(2);

		cout << "Id: " << countryId << " Name: " << countryName << " Last Update: " 
		     << lastUpdate.year << "-" << lastUpdate.month << "-" << lastUpdate.day << " " 
		     << lastUpdate.hour << ":" << lastUpdate.minute << ":" << lastUpdate.second
		     << endl;

		Statement deleteStmt(db, "delete from country where country_id = ?");
		deleteStmt.AssignNextParameter(Nullable<short int>(countryId));
		deleteStmt.Execute();
		cout << "Deleted " << deleteStmt.NumberOfAffectedRows() << " rows." << endl;
		*/

		struct stat filestat;
		if (stat("pic.jpg", &filestat) != 0) {
			cout << "Error 'stat'ing file" << endl;
			return 1;
		}

		cout << "file size is: " << filestat.st_size << endl;

		ifstream myFile("pic.jpg", ios::in | ios::binary);
		myFile.exceptions(std::ios::failbit);
		unsigned char picBuf[filestat.st_size];
		myFile.read((char *)picBuf, filestat.st_size);
		myFile.close();

		/*
		Binary pic(picBuf, filestat.st_size, filestat.st_size);
		Statement staffInsert(db, "update staff set picture = ? where staff_id = 2");
		staffInsert.AssignNextParameter(Nullable<Binary>(pic));
		staffInsert << execute;
		cout << "iUSED SENTINEL updated rows: " << staffInsert.NumberOfAffectedRows() << endl;
		*/

		Statement staffSelect(db, "select staff_id, first_name, last_name, picture from staff");
		staffSelect << execute;
		staffSelect << fetch;

		cout << "Eof: " << staffSelect.Eof() << endl;

		Nullable<char> staff_id = staffSelect.GetTinyDataInRow(0);
		Nullable<std::string> staff_first = staffSelect.GetStringDataInRow(1);
		Nullable<std::string> staff_last = staffSelect.GetStringDataInRow(2);
		Nullable<Binary> staff_pic(staffSelect.GetBinaryDataInRow(3));

		cout << "USED SENTINELS " << "id: " << staff_id << " name: " << staff_first << " " << staff_last  << " pic len: " << staff_pic.deref().BufferLength() << endl;

		Statement staff(db, "select picture from staff where first_name = ?");
		staff.AssignNextParameter(Nullable<string>("Mike"));
		staff.Execute();
		staff.FetchNextRow();
		Nullable<Binary> mikepic = staff.GetBinaryDataInRow(0);

		cout << "mikepic is: " << mikepic.deref().BufferLength() << " bytes long" << endl;

		ofstream fs("mike.jpg", ios::out | ios::binary);
		fs.exceptions(std::ios::failbit);
		fs.write((const char *)mikepic.deref().Buffer(), mikepic.deref().BufferLength());
		fs.close();
		cout << "can you open mike.jpg in a photo editor?" << endl;

	}
	catch (DatabaseException &exp)
	{
		cout << exp << endl;
	}
	catch(const std::exception &ex) {
		cout << "stl failure" << ex.what() << endl;
	}

	return 0;
}

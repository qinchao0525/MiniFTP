#include "parseconf.h"
#include "common.h"
#include "tunable.h"
#include "str.h"
//******************bool setting 
static struct parseconf_bool_setting
{
	const char *p_setting_name;
	int *p_variable;
}
//value of bool
parseconf_bool_array[] = 
{
	{"pasv_enable", &tunable_pasv_enable},
	{"port_enable", &tunable_port_enable},
	{NULL, NULL}
};

//uint type***********************
static struct parseconf_uint_setting
{
	const char *p_setting_name;
	unsigned int *p_variable;
}
parseconf_uint_array[] =
{
	{"listen_port", &tunable_listen_port},//listen port
	{"max_clients", &tunable_max_clients},//max number of clients
	{"max_per_ip", &tunable_max_per_ip},//max number of connection per ip
	{"accept_timeout", &tunable_accept_timeout},//accept timeout value
	{"connect_timeout", &tunable_connect_timeout},//connect time out
	{"idle_session_timeout", &tunable_idel_session_timeout},//
	{"data_connection_timeout", &tunable_data_connection_timeout},//
	{"local_umask", &tunable_local_umask},//
	{"upload_max_rate", &tunable_upload_max_rate},//max upload rate b/s
	{"download_max_rate", &tunable_download_max_rate},//download rate
	{NULL, NULL}//flag to stop
};

//str setting *********************************
static struct parseconf_str_setting
{
	const char *p_setting_name;
	const char **p_variable;
}
parseconf_str_array[] =
{
	{"listen_address", &tunable_listen_address},//address 
	{NULL, NULL}//flage to stop
};

//load file config function******************************
void parseconf_load_file(const char *path)
{
	FILE *fp = fopen(path, "r");
	if(fp==NULL)
		ERR_EXIT("fopen");
	
	char setting_line[1024]={0};
	while( fgets(setting_line, sizeof(setting_line), fp)!=NULL )
	{
		if(strlen(setting_line)==0 || setting_line[0] == '#'
			|| str_all_space(setting_line))
			continue;
		str_trim_crlf(setting_line);
		parseconf_load_setting(setting_line);
		memset(setting_line, 0, sizeof(setting_line));
	}	
	fclose(fp);
}

void parseconf_load_setting(const char *setting)
{
	// remove space in left
	while(isspace(*setting))
		setting++;
	//parsing value.
	char key[128]={0};
	char value[128]={0};
	str_split(setting, key, value, '=');//get value
	if(strlen(value)==0)//no value
	{
		fprintf(stderr, "missing value in config for :%s\n", key);
		exit(EXIT_FAILURE);
	}	
	//first table str
	const struct parseconf_str_setting *p_str_setting = parseconf_str_array;
	while(p_str_setting->p_setting_name != NULL)
	{
		if(strcmp(key, p_str_setting->p_setting_name)==0)
		{
			const char **p_cur_setting=p_str_setting->p_variable;
			if(*p_cur_setting)
				free((char*)*p_cur_setting);
			*p_cur_setting = strdup(value);
			return;
		}
		p_str_setting++;
	}	
	//table 2 bool
	const struct parseconf_bool_setting *p_bool_setting = parseconf_bool_array;
	while(p_str_setting->p_setting_name != NULL)
	{
		if(strcmp(key, p_bool_setting->p_setting_name)==0)
		{
			str_upper(value);
			if(strcmp(value, "YES")==0 || strcmp(value, "TRUE")==0 || strcmp(value, "1")==0)
				*(p_bool_setting->p_variable)=1;
			else if(strcmp(value, "NO")==0 || strcmp(value, "FALSE")==0 || strcmp(value, "0")==0)
				*(p_bool_setting->p_variable)=0;
			else
			{
				fprintf(stderr, "bool value in config for :%s\n", key);
					exit(EXIT_FAILURE);
			}	
			return;
		}
		p_bool_setting++;
	}	
	//table 3 uint
	const struct parseconf_uint_setting *p_uint_setting = parseconf_uint_array;
	while(p_uint_setting->p_setting_name != NULL)
	{
		if(strcmp(key, p_uint_setting->p_setting_name)==0)
		{
			if(value[0]=='0')
			{
				*(p_uint_setting->p_variable)=str_octal_to_uint(value);
			}
			else
				
				*(p_uint_setting->p_variable)=atoi(value);
			return;
		}
		p_uint_setting++;
	}	
	
}


#include "../include/URL.h"
void resetURL(url *url)
{
    memset(url->user, 0, MAX_STRING_LENGTH);
    memset(url->password, 0, MAX_STRING_LENGTH);
    memset(url->host, 0, MAX_STRING_LENGTH);
    memset(url->path, 0, MAX_STRING_LENGTH);
    memset(url->filename, 0, MAX_STRING_LENGTH);
    url->port = 21;
}

// ftp://[<user>:<password>@]<host>/<url-path>
void parseURL(url *url, const char *input_url)
{
    char start[] = "ftp://";
    int index = 0;
    enum parseState state = read_FTP;
    int length = strlen(input_url);

    for (int i = 0; i < length; i++)
    {
        switch (state)
        {
        case read_FTP:
            if (input_url[i] != start[i])
            {
                fprintf(stderr, "Failed while reading ftp://, expected: %c and got: %c.\n", input_url[i], start[i]);
                exit(-1);
            }
            else
                state = (i == 5) ? read_USR : state;
            break;

        case read_USR:
            if (input_url[i] == ':')
            {
                state = read_PASS;
                index = 0;
            }
            else url->user[index++] = input_url[i];
            break;
        case read_PASS:
            if (input_url[i] == '@')
		    	{
		    		state = read_HOST;
		    		index = 0;
		    	}
            else url->password[index++] = input_url[i];
            break;
        case read_HOST:
            if (input_url[i] == '/')
			{
				state = read_PATH;
				index = 0;
			}
            else url->host[index++] = input_url[i];
            break;
        case read_PATH:
            url->path[index++] = input_url[i];
            break;
        }
    }

    if(state != read_PATH){
        fprintf(stderr, "Something went wrong while parsing URL, state stoped at state %c\n",state);
        exit(-1);
    }

	int filename_index = 0;
	for(int indexPath = 0; indexPath < strlen(url->path); indexPath++){

		if(url->path[indexPath]=='/'){
			filename_index = 0;
			memset(url->filename, 0, MAX_STRING_LENGTH);
			
		}
		else{
			url->filename[filename_index] = url->path[indexPath];
			filename_index++;
		}
	}

}


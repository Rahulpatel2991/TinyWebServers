#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include<string.h>

#define VERSION 23
#define ERROR      42

struct file
{
	char *ext;
	char *file_type;
}extension [] = {
	{"zip","image/zip"},
	{"jpg","image/jpg"},
	{"jpeg","image/jpeg"},
	{"ico","image/ico"},
	{"tar","image/tar"},
	{"htm","text/htm"},
	{"html","text/html"},
	{0,0} 
};


void logger(int fd,int type, char *ptr,int sock_fd)
{

	switch(type)
	{
		printf("in side switch case\n");
		case ERROR:
			write(fd,ptr,strlen(ptr));
			break;

		case 403 :
			(void)write(sock_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",271); 

			write(fd,"FORBIDDEN ERROR",15);
			break;

		case 404:
			(void)write(sock_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n",224);
			write(fd,"NOT-FOUND ERROR",15);
			break;
	}
	printf("Bye Bye\n");
	close(fd);
	exit(0);
}

int main(int argc,char *argv[])
{


	int log_fd;
        log_fd =open("rweb.log", O_CREAT| O_WRONLY | O_APPEND,0644);
               
		if(log_fd == -1)
                {
                        printf("error in opening Log file\n");
                        exit(0);
                }


	if (argc!=2)
	{
		printf("Uasage:- ./rweb  port_num\n");
		logger(log_fd,ERROR,"Uasage:- ./rweb  port_num\n",0);
	}

	int sfd,nsfd,hit,len,i,j,size,fd1,file_size;
	char buffer[1024];

	struct sockaddr_in  server,client;

	sfd = socket(AF_INET,SOCK_STREAM,0);

	if(sfd < 0)
	{
		logger(log_fd,ERROR,"error in opening server Socket \n",0);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));
	server.sin_addr.s_addr = INADDR_ANY;


	if(bind(sfd,(struct sockaddr*) &server,sizeof(server)))
	{
		logger(log_fd,ERROR, "bind Error\n",0);
	}

	listen(sfd,64);

	for(hit=1;hit;hit++)
	{

		len = sizeof(client);
		int ret,len,ext;
		char hits[6]={0};
		char url[50]={0};
		char buff[1024] = {0};
		char buffer1[1024] = {0};
		char * fstr=0;
		char buffer[1024] = {0};

		len = sizeof(client);

		nsfd =  accept(sfd,(struct sockaddr *)&client,&len);

		if(nsfd < 0){
		logger(log_fd,ERROR,"Error in accept call\n",0);
		}

		read(nsfd,buff,sizeof(buff));
		printf("Data=%s\n",buff);

		size =strlen(buff);

		write (log_fd,buff,strlen(buff));

		if (strncmp(buff,"GET",3))
		{
			write(log_fd,"GET METHOD\n");	
			exit(0);
		}

		for(i=4,j=0; buff[i];i++,j++)
		{
			if (buff[i]== ' ')
				break;

			url[j] = buff[i];			
		}

		url[j] = '\0';

		printf("url=%s\n",url);

		char *ptr=0;

		ptr=strrchr(url,'.');

		if (ptr==0)
		{
			logger(log_fd,403,"file extention is not valide \n",nsfd);

		}
		else
		{
			ptr++;
		}

		ext = strlen(ptr);
		if(ext==0)
		{
                 logger(log_fd,403,"file extention is not valide \n",nsfd);
		}

		printf("len=%ld\n",strlen(ptr));

		printf("file type is =%s\n",ptr);
		fstr = 0 ;

		for(i=0;extension[i].ext!=0;i++)
		{
			if((strncmp(extension[i].ext,ptr,ext))== 0)
			{
				fstr = extension[i].file_type;
			}
		}
	
			if(fstr==0)
			{
			printf("inside loop\n");
			logger(log_fd,404,"file extention is not valide \n",nsfd);
			
			}

		
		printf("fstr=%s\n",fstr);

		fd1 =open(url, O_CREAT| O_RDONLY | O_APPEND,0644);

		if (fd1 < 0)
		{
			logger(log_fd,404,"file is NOT FOUND \n",0);
			exit(0);
		}


		file_size = lseek(fd1,0,SEEK_END);
		lseek(fd1,0,SEEK_SET);


		printf("sizeof file=%d\n",file_size);

//		(void)sprintf(buffer1,"HTTP/1.1 200 OK\nServer: nweb\nContent-Length: %d\nConnection: close\n\n", file_size);
//		printf("buffer1=%s\n",buffer1);



		
(void)sprintf(buffer,"HTTP/1.1 200 OK\nServer: nweb/%d.0\nContent-Length: %d\nConnection: close\nContent-Type: %s\n\n", VERSION, file_size, fstr); /* Header + a blank line */


		(void)write(nsfd,buffer,strlen(buffer));


		while (  (ret = read(fd1, buffer, file_size)) > 0 ) 
		{
			(void)write(nsfd,buffer,file_size);
		}
		close(nsfd);
	}
}

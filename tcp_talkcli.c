tcp_talkcli.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 511

char *EXIT_STRING="exit"; //종료문자정의
int recv_and_print(int sd); //상대로부터 메세지 수신 후 화면출력
int input_and_send(int sd); //키보드 입력받고 상대에게 메시지 전달

int main(int argc, char *argv[]){
        pid_t pid;
        static int s;
        static struct sockaddr_in servaddr;

        //명령문 입력 인자 처리
        if(argc!=3){
                printf("사용법:%s server_ip port \n",argv[0]);
                exit(0);
        }
        //소켓생성
        if((s=socket(PF_INET,SOCK_STREAM,0))<0){
                printf("Client: Can't open stream socket.\n");
                exit(0);
        }
        //servaddr을 '0'으로 초기화
        bzero((char *)&servaddr, sizeof(servaddr));
        //servaddr 세팅
        servaddr.sin_family=AF_INET;
        inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
        servaddr.sin_port=htons(atoi(argv[2]));
        //서버에게 연결 요청
        if(connect(s,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
                printf("Client: Can't connect to server.\n");
                exit(0);
        }

        if((pid=fork())>0)  //부모 프로세스
                input_and_send(s);
        else if(pid==0) //자식 프로세스
                recv_and_print(s);
        close(s);
        return 0;
}

//키보드 입력받고 상대에게 메시지 전달
int input_and_send(int sd){
        char buf[MAXLINE+1];
        int nbyte;
        while(fgets(buf, sizeof(buf),stdin) != NULL){
                nbyte=strlen(buf);
                write(sd,buf,strlen(buf));
                //종료 문자열 입력처리
                if(strstr(buf,EXIT_STRING)!=NULL){
                        puts("Good bye.");
                        close(sd);
                        exit(0);
                }
        }
        return 0;
}

//상대로부터 메세지 수신 후 화면 출력
int recv_and_print(int sd) {
        char buf[MAXLINE+1];
        int nbyte;
        while(1) {
                if((nbyte=read(sd,buf,MAXLINE))<0) {
                perror("read fail");
                close(sd);
                exit(0);
                }
                buf[nbyte] = 0;
                if(strstr(buf,EXIT_STRING) !=NULL) {
                        break;
                }
                printf("%s",buf);
        }
        return 0;
}

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include <ctype.h>

#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define ACCESS "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"

#define PORT 8080
#define HOST "34.246.184.49"
#define SIZE 1000

// citirea user-ului
char* user() {
    char username[SIZE], password[SIZE];

    printf("username=");
    fgets(username, SIZE, stdin); // citim username de la tastatura
    username[strlen(username) - 1] = '\0'; // eliminam newline-ul de la final

    printf("password=");
    fgets(password, SIZE, stdin);
    password[strlen(password) - 1] = '\0';

    // verificam daca credentialele sunt goale
    if (username[0] == '\0' || password[0] == '\0') {
        return strdup("ERROR! Credentials are empty!\n");
    }

    // verificam daca credentialele contin spatii
    if (strchr(username, ' ') != NULL || strchr(password, ' ') != NULL) {
        return strdup("ERROR! Spaces not allowed!\n");
    }

    // cream un obiect JSON pentru a stoca credentialele
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    json_object_set_string(object, "username", username);
    json_object_set_string(object, "password", password);

    // serializam obiectul JSON intr-un string
    char *serialized_string = json_serialize_to_string_pretty(value);
    return serialized_string;
}

// verifica daca stringul contine doar cifre
int is_digits_only(char str[SIZE]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0; // exista un caracter care nu e cifra
        }
    }
    return 1; // toate caracterele sunt cifre
}

// adaugam o carte noua
char* add_book() {
    char title[SIZE];
    char author[SIZE];
    char genre[SIZE];
    char publisher[SIZE];
    char page_count[SIZE];

    // citim inputul de la tastatura
    printf("title=");
    fgets(title, SIZE, stdin);
    title[strlen(title) - 1] = '\0'; // stergem newline

    printf("author=");
    fgets(author, SIZE, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    fgets(genre, SIZE, stdin);
    genre[strlen(genre) - 1] = '\0';

    printf("publisher=");
    fgets(publisher, SIZE, stdin);
    publisher[strlen(publisher) - 1] = '\0';

    printf("page_count=");
    fgets(page_count, SIZE, stdin);
    page_count[strlen(page_count) - 1] = '\0';

    // daca inputul e gol afisam mesaj de eroare
    if (title[0] == '\0' || title[0] == ' ') {
        return strdup("ERROR! Incorrect input!\n");
    }

    if (author[0] == '\0' || author[0] == ' ') {
        return strdup("ERROR! Incorrect input!\n");
    }

    if (genre[0] == '\0' || genre[0] == ' ') {
        return strdup("ERROR! Incorrect input!\n");
    }

    if (publisher[0] == '\0' || publisher[0] == ' ') {
        return strdup("ERROR! Incorrect input!\n");
    }

    if (page_count[0] == '\0') {
        return strdup("ERROR! Incorrect input!\n");
    }

    // daca page_count nu e numar afisam mesaj de eroare
    if (!is_digits_only(page_count)) {
        return strdup("ERROR! Must contain only digits!\n");
    }

    // cream un obiect JSON pentru a stoca informatiile cartii si setam inputul
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    json_object_set_string(object, "title", title);
    json_object_set_string(object, "author", author);
    json_object_set_string(object, "genre", genre);
    json_object_set_string(object, "publisher", publisher);
    json_object_set_number(object, "page_count", atoi(page_count));

    char *serialized_string = json_serialize_to_string_pretty(value);
    return serialized_string;
}

// citim id-ul de la tastatura
char* get_id() {
    char id[SIZE];
    printf("id=");
    fgets(id, SIZE, stdin);
    id[strlen(id) - 1] = '\0';

    char* str = id;
    return str;
}

// extragem cookie din raspunsul serverului
char* get_cookie(char *response) {
    char *cookie_start = strstr(response, "connect.sid=");
    if (cookie_start == NULL) {
        return NULL;
    }

    char *cookie_end = strchr(cookie_start, ';');
    if (cookie_end != NULL) {
        *cookie_end = '\0';
    }

    return strdup(cookie_start);
}

// extragem token din raspuns
char *get_token(char *response) {
    char *token_start = strstr(response, "token\":\"");
    if (token_start == NULL) {
        return NULL;
    }

    token_start += strlen("token\":\"");
    char *token_end = strchr(token_start, '\"');
    if (token_end != NULL) {
        *token_end = '\0';
    }

    return strdup(token_start);
}

// afisam cartile in format json
void print_books(char *response, char* car) {
    char *json_start = strstr(response, car);

    if (json_start != NULL) {
        JSON_Value *value = json_parse_string(json_start); // parsăm string-ul JSON
        if (value != NULL) {
            char *pretty_response = json_serialize_to_string_pretty(value);
            
            // afisam raspunsul
            printf("%s\n", pretty_response);
            // eliberam memorie
            json_free_serialized_string(pretty_response);
            json_value_free(value);
        }
    }
}

// afisam mesajul de eroare extras din raspuns
void print_error(char* response) {
    char* error = basic_extract_json_response(response);
    error += 10;
    error[strlen(error) - 2] = '\0';
    printf("ERROR! %s\n", error);
}
 
int main(int argc, char *argv[]) {
    char command[SIZE];
    char *message;
    char *response;
    int sockfd;
    char *cookies = NULL;
    char *jwt = NULL;

    int logged = 0; // variabila pentru a verifica daca userul e logat

    while (1) {
        fgets(command, SIZE, stdin);
        command[strlen(command) - 1] = '\0'; // stergem caracterul newline
        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        // comanda register
        if (strcmp(command, "register") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char* body_data = user(); // citim userul

            if (strcmp(body_data, "ERROR! Credentials are empty!\n") == 0) {
                printf("%s", body_data); // afisam mesajul de eroare
                free(body_data);
                close_connection(sockfd);
                continue;
            }

            if (strcmp(body_data, "ERROR! Spaces not allowed!\n") == 0) {
                printf("%s", body_data); // afisam mesajul de eroare
                free(body_data);
                close_connection(sockfd);
                continue;
            }

            // cream cererea POST pentru inregistrare
            message = compute_post_request(HOST, REGISTER ,"application/json", &body_data, 1, NULL, 0, NULL);
            send_to_server(sockfd, message); // trimitem cererea la server
            response = receive_from_server(sockfd); // primim raspunsul de la server

            if (strstr(response, "201 Created") != NULL) {
                printf("SUCCESS! User created!\n"); // afisam mesajul de succes
            } else {
                print_error(response); // afisam mesajul de eroare
            }
            close_connection(sockfd);

        // comanda login            
        } else if (strcmp(command, "login") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            char* body_data = user();

            if (strcmp(body_data, "ERROR! Credentials are empty!\n") == 0) {
                printf("%s", body_data); // afisam mesajul de eroare
                free(body_data);
                close_connection(sockfd);
                continue;
            }

            if (strcmp(body_data, "ERROR! Spaces not allowed!\n") == 0) {
                printf("%s", body_data); // afisam mesaj erosre
                free(body_data);
                close_connection(sockfd);
                continue;
            }

            message = compute_post_request(HOST, LOGIN, "application/json", &body_data, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            if (strstr(response, "200 OK") != NULL) {
                cookies = get_cookie(response); // extragem cookie-ul din raspuns
                printf("SUCCESS! Welcome!\n"); // afisam mesajul de succes
                logged = 1; // setam flag-ul de logare
            } else {
                print_error(response);
            }

            close_connection(sockfd);
        
        // comanda logout 
        } else if (strcmp(command, "logout") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // verificam daca userul e logat
            if (logged) {
                // cream cererea GET pentru deconectare
                message = compute_get_request(HOST, LOGOUT, NULL, &cookies, 1, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (strstr(response, "200 OK") != NULL) {
                    printf("SUCCESS! The user has successfully logged out!\n"); // afisam mesajul de succes
                    logged = 0; // resetam flag-ul de logare
                } else {
                    print_error(response); // afisam mesajul de eroare
                }

                // eliberam memoria
                free(cookies);
                cookies = NULL;
                free(jwt);
                jwt = NULL;
            } else {
                printf("ERROR! The user is not logged in!\n"); // afisam mesajul de eroare daca userul nu e logat
            }

            close_connection(sockfd);

        // comanda enter_library
        } else if (strcmp(command, "enter_library") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (logged) {
                // cream cererea GET pentru accesare biblioteca
                message = compute_get_request(HOST, ACCESS, NULL, &cookies, 1, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                jwt = get_token(response); // extragem token-ul din raspuns

                if (strstr(response, "200 OK") != NULL) {
                    printf("SUCCESS! The user has access to the library!\n");
                } else {
                    print_error(response);
                }
            } else {
                printf("ERROR! The user is not logged in!\n"); // afisam eroare daca utilizatorul nu este logat
            }

            close_connection(sockfd);

        // comnada get_books
        } else if (strcmp(command, "get_books") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (logged) {
                message = compute_get_request(HOST, BOOKS, NULL, &cookies, 1, jwt);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (strstr(response, "200 OK") != NULL) {
                    print_books(response, "["); // afisam lista de carti
                } else {
                    print_error(response);
                }
            } else {
                printf("ERROR! The user is not logged in!\n");
            }

            close_connection(sockfd);

        // comanda get_book    
        } else if (strcmp(command, "get_book") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (logged) {

                char* id = get_id(); // citim id-ul cartii de la tastatura
                // verificam daca id-ul e gol                
                if (id[0] == '\0') {
                    printf("ERROR! The id is empty!\n");
                    close_connection(sockfd);
                    continue;   
                }
                // verificam daca id-ul contine spatii
                if (strchr(id, ' ') != NULL) {
                    printf("ERROR! Spaces not allowed!\n");
                    close_connection(sockfd);
                    continue;   
                }


                char str[SIZE] = "/api/v1/tema/library/books/";
                strcat(str, id); // formam url-ul pentru cerere
                
                message = compute_get_request(HOST, str, NULL, &cookies, 1, jwt);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (strstr(response, "200 OK") != NULL) {
                    print_books(response, "{"); // afisam detaliile cartii
                } else {
                    print_error(response);
                }

            } else {
                printf("ERROR! The user is not logged in!\n");
            }

            close_connection(sockfd);
        
        // comanda add_book
        } else if (strcmp(command, "add_book") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            
            if (logged) {
                char* body_data = add_book(); // citim datele cartii

                // afisam mesajul de eroare
                if (strcmp(body_data, "ERROR! Incorrect input!\n") == 0) {
                    printf("%s", body_data); // afiseaza mesajul de eroare
                    free(body_data);
                    close_connection(sockfd);
                    continue;
                }

                if (strcmp(body_data, "ERROR! Must contain only digits!\n") == 0) {
                    printf("%s", body_data);
                    free(body_data);
                    close_connection(sockfd);
                    continue;
                }

                // cream cererea POST pentru adaugare carte
                message = compute_post_request(HOST, BOOKS, "application/json", &body_data, 1, &cookies, 1, jwt);
                free(body_data);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (strstr(response, "200 OK") != NULL) {
                    printf("SUCCESS! The book is added!\n");
                } else {
                    print_error(response);
                }
            } else {
                printf("ERROR! The user is not logged in!\n");
            }
            close_connection(sockfd);

        // comanda delete_book
        } else if (strcmp(command, "delete_book") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            if (logged) {

                char* id = get_id(); // citim id-ul cartii de la tastatura
                // verificam daca id-ul e gol
                if (id[0] == '\0') {
                    printf("ERROR! The id is empty!\n");
                    close_connection(sockfd);
                    continue;   
                }
                // verificam daca id-ul contine spatii
                if (strchr(id, ' ') != NULL) {
                    printf("ERROR! Spaces not allowed!\n");
                    close_connection(sockfd);
                    continue;   
                }

                char str[SIZE] = "/api/v1/tema/library/books/";
                strcat(str, id); // formam url-ul pentru cerere
                
                // cream cererea DELETE pentru stergere carte
                message = compute_delete_request(HOST, str, NULL, &cookies, 1, jwt);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);

                if (strstr(response, "200 OK") != NULL) {
                    printf("SUCCESS! The book is deleted!\n");
                } else {
                    print_error(response);
                }
                
            } else {
                printf("ERROR! The user is not logged in!\n");
            }
            close_connection(sockfd);

        // comanda exit 
        } else if (strcmp(command, "exit") == 0) {
            break;

        // comanda invalida
        } else {
            printf("Invalid command!\n");
        }

        close_connection(sockfd); // inchidem conexiunea la server
    }

    return 0;
}

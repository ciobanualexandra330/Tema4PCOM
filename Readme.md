-- Client Web. Comunicatie Cu REST API --

Ciobanu Alexandra 324CD

Baza temei este laboratorul 9 la care am adaugat functia 
'compute_delete_request' si am adaugat parametrul token la functiile de 
POST si GET

Comenzi:
    >register:
    - citesc datele de la consola
    - verific daca username-ul sau parola nu sunt goale sau nu au spatii
    - creez un obiect JSON pentru a stoca informatiile cartii si setez 
    
    >inputul
    - compun un mesaj de post si trimit mesajul la server
    - din raspunsul primit extrag fie mesajul de succes (201 Created), 
    fie mesaj de eroare
    
    >login:
    - citesc datele de la consola
    - verific daca username-ul sau parola nu sunt goale sau nu au spatii
    - creez un obiect JSON pentru a stoca informatiile cartii si setez inputul
    - salvez cookieul
    - din raspunsul primit extrag fie mesajul de succes (200 OK), fie
    mesaj de eroare
    
    >logout
    - verific daca user-ul e logat, in caz contrar afisez eroare
    - eliberez memoria pentru cookies si token
    - inchid conexiunea
    
    >enter_library
    - trimit mesaj de GET cu cookie-ul salvat
    - extrag tokenul din raspuns
    - din raspunsul primit extrag fie mesajul de succes (200 OK), fie
    mesaj de eroare
    
    >get_books
    - verific daca utilizatorul e logat
    - trimit mesaj de GET cu cookie-ul salvat
    - din raspunsul primit extrag fie mesajul de succes (200 OK), fie
    mesaj de eroare
    
    >get_book
    - verific daca utilizatorul e logat
    - citesc de la tastatura id-ul si verific daca nu e gol sau daca nu
    are spatii
    - creez un url nou pentru cartea cu id-ul citit
    - trimit mesaj de GET cu url-ul creat si cookieul salvat
    - din raspunsul primit extrag datele despre carte si le afisez
    
    >add_book
    - verific daca utilizatorul e logat
    - citesc datele cartii si le formatez json
    - trimit mesaj de POST cu datele cartii
    - din raspunsul primit extrag fie mesajul de succes (200 OK), fie
    mesaj de eroare
    
    >delete_book
    - verific daca utilizatorul e logat
    - citesc de la tastatura id-ul si verific daca nu e gol sau daca nu
    are spatii
    - creez un url nou pentru cartea cu id-ul citit
    - trimit mesaj de DELETE cu url-ul creat si cookieul salvat
    
    >exit
    - termin executia programului
    
Functii auxiliare:
    char* user():
    - citesc username si password de la user, validez inputurile si 
    returnez un string json serializat care contine credentialele

    int is_digits_only(char str[SIZE]):
    - verific daca stringul contine doar cifre

    char* add_book():
    - citesc datele unei carti de la user, validez inputurile si 
    returnez un string json serializat care contine detaliile cartii

    char* get_id():
    - citesc id-ul utilizatorului

    char* get_cookie(char *response):
    - extrag un cookie din raspunsul serverului

    char *get_token(char *response):
    - extrag un token din raspunsul serverului

    void print_books(char *response, char* car):
    - extrag si afisez informatii despre carti/carte in format json

    void print_error(char* response):
    - extrag si afisez mesajul de eroare din raspunsul serverului
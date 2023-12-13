# TP2ProgSysJOUVE_PICHT

## Question 1  

<img width="221" alt="Capture d’écran 2023-12-13 à 16 33 04" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/20503aff-dd86-456f-bc0e-a99547916e81">  

On passe les informations de la ligne de commande dans la chaine de caractère argv.  

---
## Question 2  

On utilise un pointeur vers une structure addrinfo permettant de forcer l'IPV4 ainsi que de prioriser TFTP sur UDP. On configure alors getaddrinfo qui permet d'obtenir la liste des adresse IP et port du serveur. 

<img width="544" alt="Capture d’écran 2023-12-13 à 16 31 49" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/1e9a004f-3a52-4628-83fd-c34240c7bd13">  

getaddrinfo renvoyant une valeur (0 si aucune erreur), on teste la sa valeur pour afficher le message d'erreur. 
<img width="466" alt="Capture d’écran 2023-12-13 à 16 38 02" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/9b951485-44f8-4613-9a6a-b29bcb2ca503">  

---  
## Question 3

On créer la liaison entre le client et le serveur (socket) selon les informations de requêtes obtenues à la Q1 (ai_family, ai_socktype, ai_protocol). Cela se fait à l'aide de la fonction socket() de socket.h.  

<img width="545" alt="Capture d’écran 2023-12-13 à 16 44 16" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/a225cfeb-ff7f-4752-90cb-8b7aadd690cd">  

Bien entendu, on teste la valeur de retour de la fonction (-1 si erreurs) et on renvoie une erreur si besoin.  


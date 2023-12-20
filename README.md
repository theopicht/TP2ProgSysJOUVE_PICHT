# TP2ProgSysJOUVE_PICHT

L'objectif de ce TP et de créer un échange client-serveur (pour échanger des fichiers) via le protocole TFTP (niveau 4 dans le modèle TCP/IP). Nous utiliserons donc les RFC qui sont des documents numérotées contenant des informations sur la manière d'utiliser le protocole TFTP.  
L'objectif sera de gérer deux commandes :  
- gettftp host file  
- puttftp host file


## Question 1  

<img width="221" alt="Capture d’écran 2023-12-13 à 16 33 04" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/20503aff-dd86-456f-bc0e-a99547916e81">  

On passe les informations de la ligne de commande dans la chaine de caractère argv. Les informations extraites sont le nom du serveur ainsi que le nom du fichier à manipuler.  

---
## Question 2  

On utilise un pointeur vers une structure addrinfo permettant de forcer l'IPV4 ainsi que de prioriser TFTP sur UDP. Le champ ai_amily contient le type d'adresse, ai_socktype contient le type de socket, soit le mode de fonctionnement que l'on peut utiliser, et à ai_protocol contient le protocole IPV4 que l'on veut utiliser pour établir une connexion avec le serveur. On configure alors getaddrinfo qui permet d'obtenir la liste des adresse IP et port du serveur. 

<img width="544" alt="Capture d’écran 2023-12-13 à 16 31 49" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/1e9a004f-3a52-4628-83fd-c34240c7bd13">  

getaddrinfo renvoyant une valeur (0 si aucune erreur), on teste la sa valeur pour afficher le message d'erreur. 
<img width="466" alt="Capture d’écran 2023-12-13 à 16 38 02" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/9b951485-44f8-4613-9a6a-b29bcb2ca503">  

---  
## Question 3

On crée la liaison entre le client et le serveur (socket) selon les informations de requêtes obtenues à la Q1 (ai_family, ai_socktype, ai_protocol). Cela se fait à l'aide de la fonction socket() de socket.h.  

<img width="545" alt="Capture d’écran 2023-12-13 à 16 44 16" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/a225cfeb-ff7f-4752-90cb-8b7aadd690cd">  

Bien entendu, on teste la valeur de retour de la fonction (-1 si erreurs) et on renvoie une erreur si besoin.  

---
## Question 4

On cherche donc à créeer la requête en lecture (RRQ) et à l'envoyer au serveur. Pour cela, on sait que le RRQ à créer respecte le format ci dessous (récupéré dans la RFC 1350).  

<img width="506" alt="Capture d’écran 2023-12-15 à 19 21 44" src="https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/861cfef1-1f60-48ce-9ca0-6c7630c346c1">  

Ici, l'opcode correspond à 1, puisque l'on souhaite adresser une RRQ. Le 1er string correspond au nom du fichier à lire sur le serveur et le 2ème correspond au mode, ici le mode "octet" qui permet d'envoyer caractères et images.  
On utilise alors la fonction memcpy qui permet de copier l'espace mémoire souhaité, ainsi que strcpy qui permet de copier le contenu du tableau dans le paquet RRQ. On initialise aussi une variable contenant la longueur du RRQ.  
Pour envoyer le message, on utilise sendto() (avec implémentation du ERROR_HANDLER).  

Pour la réception du message, on créer une structure. Après avoir alloué la mémoire nécessaire et créer un HANDLER pour s'assurer que le fichier est bien recu, on recoit les bits du fichier à l'aide de la fonction recvfrom. Cette fonction prend en argument le socket, son buffer (soit le nombre de bits à recevoir à chaque appel), sa taille et la structure pour stocker le retour. Cette fonction renvoie la longueur du message recu, on vérifie donc que ce nombre n'est pas nul. Puis, à l'aide de "buffer[BUFFER_OFFSET] << 8", on fait un décalage de 8 bit puis un "OU" pour transformer les 2 octets de base en un entier de 16 bit en respectant la considération LSB. On lis ensuite le fichier block de 516 octets par block de 516 octets (512 octets pour les données + 4 pour le header), en les réécrivant dans un fichier local (qui sera donc une copie bit à bit du fichier recu, de la part du serveur).  

Enfin, on envoie un dernier message au serveur pour confirmer la bonne réception du fichier (ACK = n° ACK avant + 1) à l'aide sendto().

<ins>Résultats </ins> :    

On utilise un logiciel permettant de téléverser un fichier sur notre serveur. Ici seulement un fichier de 59 octets.  

![Capture d’écran 2023-12-13 à 16 28 29](https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/38427851-aeff-49bc-a4c5-e8f8660aad29)

Après appel à la fonction gettftp, on observe bien les 3 échanges de données sur WireShark (requête RRQ, lecture fichier, messagee ACK). Le fichier est bien copié sur notre répertoire local courant.  


En effectuant la même oppération avec un fichier plus lourd (ici 48.4MB), on parvient aussi à copier le fichier.  

![Image PNG](https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/636be7a3-dca0-4fe2-85ef-37f19a381127)


![Image PNG](https://github.com/theopicht/TP2ProgSysJOUVE_PICHT/assets/151057454/8cc39a13-7f45-4bf4-87ff-9f53798f3942)  

WireShark nous montre bien la transmission de plusieurs blocs.  


---
## Question 5





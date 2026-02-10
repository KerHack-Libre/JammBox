<img src="assets/logos/freedos.jpg" width=""  height="" alt="freedos" style="margin-right:20px;border-radius:35px"  align="right"/>
<img src="assets/logos/dosbox-logo.png" width="132"  height="" alt="dosbox" style="margin-right:20px;border-radius:35px"  align="left"/>
<div>
<div>
</div>

 <a href="https://github.com/KerHack-Libre/https://github.com/KerHack-Libre/JammBox/"><img src ='https://img.shields.io/badge/JammBox-teal?style=for-the-badge&logo=appveyor'/></a>


---

## Pourquoi  "jàmm"

Parce "jàmm", c’est avant tout : Paix, détente, fun.
Ceci est  une petite invitation à souffler en replongeant dans des jeux simples, directs, addictifs… comme avant.

## Dallal Ak Jàmm 
Ce dépôt est conçu pour (re)découvrir le charme intemporel des jeux retros DOS classiques, préconfigurés pour tourner directement via DOSBox.
Que tu sois nostalgique de l’ère MS-DOS ou simplement curieux de l’histoire ludique, ce projet te plonge dans un petit voyage rétro — simple, rapide, et surtout… amusant. 

🎯 Objectif du projet

Offrir une collection de jeux DOS prêts à l’emploi, configurés pour se lancer automatiquement.
Préserver et partager l’esprit des jeux “à l’ancienne”, accessibles.
Proposer une expérience.(qui inspireront d'autre currieux a decouvrire  l'informatique).

Parfait pour un petit moment de detente et d'amusement a tout ages.

Une Seul chose pour Jouer convenablement:est d'avoir l'emulateur DosBox disponible. (c'est tout). 


### Contenu du depot 
```bash 
/games/ : les jeux DOS configurés
/src/   : Programme source en C
/assets/: Les assets 
  misc/ : divers
  imgs/ : Images & logos 

README.md : tu es ici 👋
```
## Comment utiliser  
la methode la plus simple est de cloner simplement le depot 
car vous pouvez mettre a joure facilement le depot avec un git pull. 
pour avoir les derniers jeux. 


```bash 
git clone <url_du_repo>
cd Jammbox

avant de compiler assurer d'avoir libzip et curses disponible pour une meilleur experience 
car ils gerer  le compression et la decompression des jeux et l'affichage du menu. 

meson setup build 
meson compile -C build 
```
pour le lancer: 
faite juste : ./build/Jammbox 
Puis laisse la magie DOSBox opérer. 

Pour le moment voici la list des jeux disponible 

Jeux disponibles: 
- Prince of Persia (1990) ✔️
- Prince of Persia 2 <<The Shadow and the Flame>> 
- Pacman <<Ms. Pac-Man Maze Madnes>> 

*Pour le moment le projet est toujours en amelioration continue et d’autres jeux serons bientôt  disponible 
du genre : classiques d’action, plateforme, puzzle, aventure, etc....*   

vous pouvez soumettre un pull-request 
- une idee 
- un jeux que vous voulez jouer 
- issue , bug fix , amelioration 
- ou discuter sur un sujet technique si cela vous interress. 

je suis ouvert a toute recommandation et je reste a l'ecoute. 


### Auteur & Mainteneur
Umar Ba [jUmarB@protonmail.com](jUmarB@protonmail.com) 
_KerHack-Libre_ — “comprendre, construire, transmettre.”

### ⚖️ Licence
 <a href="https://github.com/KerHack-Libre/https://github.com/KerHack-Libre/JammBox/"><img align="right" src ='https://img.shields.io/badge/JammBox-teal?style=for-the-badge&logo=appveyor'/></a>

_L’ensemble des projets de KërNix sont distribués sous GPLv3,
en accord avec les 4 libertés fondamentales du logiciel libre_ 


<img src="assets/logos/freedos.jpg" width=""  height="" alt="freedos" style="margin-right:20px;border-radius:35px"  align="right"/>
<img src="assets/logos/dosbox-logo.png" width="132"  height="" alt="dosbox" style="margin-right:20px;border-radius:35px"  align="left"/>
<div>
<div>
</div>

 <a href="https://github.com/KerHack-Libre/https://github.com/KerHack-Libre/JammBox/"><img src ='https://img.shields.io/badge/JammBox-teal?style=for-the-badge&logo=appveyor'/></a>


---

## Pourquoi  "jàmm"

Parce "jàmm", c’est avant tout : Paix, détente, fun.
Et une petite invitation à souffler en replongeant dans des jeux simples, directs, addictifs… comme avant.

## Dallal Ak Jàmm 
Ce dépôt est conçu pour (re)découvrir le charme intemporel des jeux DOS classiques, préconfigurés pour tourner directement via DOSBox.
Que tu sois nostalgique de l’ère MS-DOS ou simplement curieux de l’histoire ludique, ce projet te plonge dans un petit voyage rétro — simple, rapide, et surtout… amusant.

🎯 Objectif du projet

Offrir une collection de jeux DOS prêts à l’emploi, configurés pour se lancer automatiquement.
Préserver et partager l’esprit des jeux “à l’ancienne”, accessibles en un clic.
Proposer une expérience.(qui inspireront d'autre currieux a decouvrire  l'informatique) 

En développant ce programme, j’ai plongé dans les mécanismes internes des vieux systèmes DOS, et notamment :
- Le fonctionnement des CHS (Cylinder / Head / Sector),
- Le décodage des adresses physiques du disque,
- La lecture et bidouille du Master Boot Record,
- L’analyse des tables de partition pour reconstituer les structures internes.
- Exploration des systemes de fichier FAT16 et 32 
_(Documentation en cours de redaction)_  

Un excellent prétexte pour apprendre, expérimenter, et comprendre comment les PC de l’époque démarraient, organisaient les données… et lançaient nos jeux préférés.

> [!IMPORTANT]
> Pour Jouer convenablement  il est recommande  d'avoir l'emulateur DosBox disponible.

### Contenu du depot 
```bash 
/games/ : les jeux DOS configurés
/src/   : Programme source en C
/assets/: Les assets 
  misc/ : divers
  imgs/ : Images & logos 

README.md : tu es ici 👋
(Et d’autres fichiers techniques ou utilitaires selon les ajouts)
```

Jeux disponibles: 
- Prince of Persia (1990) ✔️
- … et d’autres bientôt : classiques d’action, plateforme, puzzle, aventure, etc.
(Peut-être même quelques surprises pour les puristes 🎩) 

## Comment utiliser  

Clone simplement le depot 

```bash 
git clone <url_du_repo>
cd Jammbox
meson setup build 
meson compile -C build 

#Dezip le jeu et lance le ! 
unzip  games/dossier_du_jeu/jeu.zip       
./build/Jbox games/dossier_du_jeu/jeu.img 
```
Puis laisse la magie DOSBox opérer.

### Auteur & Mainteneur
Umar Ba [jUmarB@protonmail.com](jUmarB@protonmail.com) 
_KerHack-Libre_ — “comprendre, construire, transmettre.”

### ⚖️ Licence
 <a href="https://github.com/KerHack-Libre/https://github.com/KerHack-Libre/JammBox/"><img align="right" src ='https://img.shields.io/badge/JammBox-teal?style=for-the-badge&logo=appveyor'/></a>

_L’ensemble des projets de KërNix sont distribués sous GPLv3,
en accord avec les 4 libertés fondamentales du logiciel libre_ 


# MSM TR2 — Contrôle A-01 intermédiaire de l'essai R3-8H : durée réussie, qualité non qualifiée

**Date : 30 octobre 2026 — ChatGPT. État : revue intermédiaire et non finale. Mission `TR2-20261030-R3-8H-FAT32-AUTONOME` en rapport `BLOCKED`. Aucune qualification produit ni tag `tr2/Mnnn`.**

## Sources et identité

Branche `main` consultée au HEAD `fa7a8e62473cdcb34aa147f9cae9988ee5c31a20`, gouvernance `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole V2, contrat courant, `DERNIER_RAPPORT.md` blob `33402b2e8c15c5e98ed5cf247f7824b8b5f52851`, roadmap et sources `experiments/r3_durable_fat32/src/main.c`, importeur strict `tools/read_campaign.py`, et outil forensique `tools/inspect_quality_failure.py` relus. Les commits techniques `1d3a5763c34649d36b216b8ab1a1b894ae0a1210` (profil 8h réellement exécuté selon Codex), `ed7f89ecf48dcb0c16c9fc7b96aaee69b16b6ccb` (preuves terminales), `bc0f02f05e1ce7a67981975bfd0942dcd835fc62` (correctif qualité **non flashé**) et `fa7a8e62473cdcb34aa147f9cae9988ee5c31a20` (checkpoint) ont été retrouvés sur le dépôt distant.

## Constat qualifié à ce stade

Le rapport Codex et ses artefacts décrivent une acquisition TR2-002 réellement autonome après déclenchement et déconnexion de Codex/GDB ; QPC Windows natif indépendant mesure un **intervalle entièrement intérieur à une même session active de 28 801,0888096 secondes**, puis STOP autonome et microSD quiescente observés. Le statut de la mission reste néanmoins `BLOCKED` dans l'attente de transfert physique microSD et d'extraction PC : **ChatGPT n'a pas indépendamment relu le fichier original sur T1000**, et ne prétend donc pas avoir qualifié son intégrité de bout en bout.

**Diagnostics terminal STM32 déclarés :** 808 679 192 XYZ natifs, 80 867 920 sorties FIR /10, **485 207 520 octets utiles** produits ; aucune perte FIFO, file RAM, SD ni défaut temporel rapporté ; FIFO pic51, file RAM pic8 192/65 536B, STOP150µs, drain17566µs. `rails=0`, **`clips=1`**, `clock_fault=0`. Contre-calculs ChatGPT : `ceil(808679192/10)=80867920`, `80867920×6=485207520`. **Ce n'est PAS une saturation du débit de la carte ni de la file de données ; c'est un écrêtage numérique détecté à la sortie du FIR**, sans attribution physique définitive à ce stade.

**Anomalie logicielle identifiée** : le firmware exécuté a écrit un pied de fichier `COMPLETE` en présence de `clips=1`, alors que le lecteur PC strict existant rejette `rails/clips` ou le drapeau par bloc `OUTPUT_CLIP`. Le nouveau correctif `bc0f02f0` ajoute un gate **après drain et avant footer** qui refuse un succès lorsque `rails||clips` et conserve l'archive INCOMPLETE ; **il n'empêche ni ne corrige l'écrêtage FIR en lui-même**. Il n'a pas été flashé et ne rétroagit pas sur les données précédentes.

Codex a démontré avec une **entrée de synthèse**, et non avec les 107 sources réelles non archivées autour de l'événement, qu'un FIR Q15 dont `sum(abs(coefficients))=47468/32768≈1,4486` peut produire une sortie >32767 sans qu'aucune source ne soit exactement aux rails ADC. Cette démonstration prouve une **possibilité technique**, pas la cause physique de l'unique événement observé.

## Décision de pilotage et limites

**Ne pas qualifier l'essai R3-8H comme complet** tant que l'extraction intégrale PC et la qualité ne sont pas démontrées ; préserver le fichier original, la SHA producteur RAM et l'ensemble des diagnostics/QPC. Ne pas assouplir l'importeur, effacer un drapeau, transformer une sortie clippée en valeur calibrée, augmenter la taille de la queue à l'aveugle ou ignorer le signal qualité. Aucune justification pour changer SPI/SD/FIFO à cause de cet unique `OUTPUT_CLIP`.

L'utilisateur a **explicitement demandé le 30/10/2026 une nouvelle acquisition 8h** après cette tentative. En conséquence : **autoriser la reprise sous le même `mission_id`**, d'abord par extraction PC et localisation exacte du bloc/axe/temps du clip et contrôle SHA de tous les octets utiles ; ensuite correction expérimentale minimale, motivée par les preuves et contrôlée sur un test court (incluant l'absence de régression spectrale), puis un **seul** nouveau long essai 8h, avec nom de fichier vierge et débit/temps/qualité stricts. Si une modification structurante du format, de la sensibilité ou de la politique d'acceptation est incontournable, remonter à ChatGPT/utilisateur au lieu de la faire implicitement. Pas de campagne longue identique sans plan causal crédible.

**Intervention physique encore nécessaire :** dernier état Codex réellement rapporté = FIFO STOP, writer inactif, SD quiescente et GDB détaché ; **aucune confirmation de microSD actuellement placée au lecteur T1000**. L'utilisateur seul coupe l'alimentation de TR2-002 et transfère la microSD après état sûr. Pas de manipulation mécanique de carte ni écriture avant cette confirmation.

**Niveau de revue** : conformité horaire et terminale étayée par le rapport et les contrôles documentaires, stockage complet **non vérifié PC**, qualité du signal **défaillante sur 1 sortie**. La revue A-01 terminale est différée. Les acquis de l'heure précédente R3-1H restent qualifiés.

---
mission_id: TR2-20261011-RS485-RX-BRINGUP-008
status: READY
base_ref: main
created_at_utc: 2026-10-11T14:00:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# Mission 008 — Correction contrôlée du bring-up RX RS-485 VDDIO2 et DE PG4

## Source de vérité et faits physiques nouveaux

Relire impérativement AGENTS.md, protocole V2-B3, mission/rapport 007, DIAGNOSTIC_VDDIO2_RS485_007.md, documentation de câblage/pinout du dépôt, schéma officiel MIKROE-3863 (ADM2867E), et code courant sur main. Vérifier HEAD réel, état Git local/distant, préserver stashes et travaux utilisateur.

Observations du banc communiquées directement par l'utilisateur le 11/10/2026 :
- NUCLEO-U575ZI-Q, alimentation logique 3,31 V.
- SB28 **fermé**, SB29 **ouvert** ; tension **3,31 V aux extrémités de SB28** ; SB29 présente 3,3 V d'un côté et 0 V de l'autre (ne pas déduire un défaut de rail VDDIO2 de SB29 ouvert).
- **DE du breakout relié physiquement à PG4** ; **/RE non connecté**, **PG5 non connecté**. L'utilisateur rappelle un **pont soudé sur J2 du breakout** à la demande d'une étape précédente. Identifier précisément la fonction réelle de J2, la révision de la carte, la différence éventuelle J2/JP2/J3 et l'état de /RE selon le schéma et les instructions du dépôt ; ne jamais inventer un fil PG5.
- **PG4 mesuré à 0,88 V par rapport à GND** au repos avec montage alimenté ; ce n'est pas une preuve que DE est franchement bas. La mesure peut refléter une sortie flottante ; ne pas conclure sans code/schéma.
- Lecture GDB antérieure PWR_SVMCR.IO2SV bit29 = 0 dans un contexte donné, firmware réellement en Flash **non identifié** et différent du BIN006. Aucun RX physique qualifié. Session GDB fermée selon utilisateur, OpenOCD potentiellement actif. microSD absente.
- HP FT232R RS485 COM7, 115200 8E1, trame test unique `01 03 03 E8 00 01 04 7A` ; firmware RX diagnostic n'est pas censé répondre à la requête.

## Objectif prioritaire

Implémenter **la correction logicielle minimale et sûre** pour rendre le firmware STM32 RX diagnostique prêt à une qualification physique future, en maîtrisant l'état de DE/PG4 et la levée d'isolation VDDIO2 avant configuration PG7/PG8 LPUART1. Ne pas faire de validation physique fictive, ne pas flasher, ne pas transmettre sur COM7.

## Travail autonome

1. Inspecter le pinout et les schémas, notamment la fonction exacte du pont J2 soudé et la stratégie /RE réellement prévue. Si l'état de /RE ne peut être garanti sans mesure ou information supplémentaire, documenter explicitement cette limite, sans recâblage inventé ni blocage artificiel de tout travail logiciel.
2. Vérifier le code STM32 et la HAL officielle : initialisation GPIOG/PG4, polarité DE, VDDIO2 IO2SV, PG7/PG8 AF8 LPUART1, ordre de boot, et toute autre fonction qui pourrait écrire PG4 ou IO2SV. **Ne pas utiliser PG5 pour /RE**. Conserver les règles de sûreté électrique et ne pas activer l'émission.
3. Corriger si techniquement établi : configurer PG4 en sortie à niveau **bas explicite avant de rendre la sortie active** (précharger ODR bas puis mode output, éviter impulsion haute), activer VDDIO2 par HAL officielle une fois le rail physiquement confirmé, puis configurer PG7/PG8 et UART. Évaluer l'impact d'IO2SV sur tout PG2..PG15, et documenter le risque de transitoire au reset avant que le firmware ne prenne PG4 en charge. Pas de TX fonctionnel ni commande DE haute dans cette tranche. Si le matériel/schéma impose une séquence différente, documenter les preuves et choisir la solution sûre.
4. Ajouter tests ciblés et/ou assertions vérifiables (ordre DE bas → VDDIO2 → AF UART, non-régression), sans prétendre tester l'électronique par logiciel. Conserver mapping gelé, services, interfaces et dettes microSD E4.
5. Cross-build ARM et tests proportionnés suivant AGENTS.md ; documenter fichiers, sorties, SHA256 BIN/ELF, limites, et préparer **procédure utilisateur courte et un seul essai HP** à effectuer plus tard après flash explicitement approuvé. Ne pas utiliser des symboles GDB d'un ELF différent du firmware Flash.
6. Inspecter diff, secrets, destination main et état distant ; commit/push normaux autorisés ; relecture distante des livrables et rapport final.

## Autorisations

Développement, documentation, compilation, tests, commit et push normaux sur **main** autorisés de manière permanente et expressément dans ce contrat : `target_branch: main`, `allow_commit: true`, `allow_push: true`.

**Interdits :** aucun flash, debug cible, OpenOCD/GDB, reset, écriture registres, émission COM7, changement de pont J2/SB28/SB29, manipulation physique, Option Bytes, mass erase, force push, réinitialisation Git destructive. Toute intervention physique reste à l'utilisateur.

## Critères d'acceptation et sortie

- PG4=DE, PG5 libre, /RE non raccordé respectés partout.
- Rôle de J2 vérifié avec source ou marqué non confirmé, sans extrapolation.
- Correctif minimal réellement compilé, ou blocage précisément justifié avec patch prêt et conditions explicites.
- Pas de faux résultat de réception, pas de flash ; distinguer COMPILÉ STM32, NON FLASHÉ, NON QUALIFIÉ RX.
- Rapport `Modbus RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md` sous cet identifiant, avec commits, tests, empreintes, limites, et procédure physique concise.
- Vérifier la publication distante sur main, indiquer SHA final ; ChatGPT réalisera ensuite un contrôle indépendant.

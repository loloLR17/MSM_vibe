---
mission_id: TR2-20261012-M015-K-ST-OFFICIAL-REFERENCE
status: READY
base_ref: main
base_sha: 0b76875c1a0257362280780f835991395b10fcbe
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-K — Banc IIS3DWB indépendant, drivers et démos officiels ST

## Objectif
Préparer un projet STM32U575ZI-Q indépendant pour le second NUCLEO et un breakout IIS3DWB seul, en utilisant les sources officielles STMicroelectronics, sans reproduire les choix du harness TR2. L'utilisateur monte actuellement le banc ; **ne pas flasher ni lancer de campagne matérielle tant que le câblage réel et l'identité de la seconde carte n'ont pas été confirmés**. Préparation logicielle autonome immédiate.

## Références et audit
Lire AGENTS.md V2-B3, rapport M015-J BLOCKED, M015-I DONE, état courant et roadmap sur main réel. Archiver mission/rapport précédents sans écrasement des preuves. Récupérer depuis sources officielles ST le pilote IIS3DWB (STMems_Standard_C_drivers/iis3dwb_STdC et exemples iis3dwb_read_data_polling.c / iis3dwb_fifo.c ou équivalents réels), figer URL, commit/tag, licence et SHA des sources, et consigner précisément les adaptations. Vérifier datasheet IIS3DWB, schéma NUCLEO-U575ZI-Q et pinout matériel réel du breakout. Ne pas inventer de broches : fournir une table pin-à-pin NUCLEO connecteur/broche STM32 ↔ breakout avec justification schéma, GPIO CS, SPI MOSI/MISO/SCK, 3V3, GND, tensions, polarité CS, mode SPI et absence de conflit. Séparer les broches proposées des broches effectivement câblées par l'utilisateur.

## Réalisation
1. Créer un projet CMake/STM32CubeU5 autonome et isolé du firmware TR2, avec drivers ST officiels conservés au maximum, portage read_reg/write_reg/delay/memoriaux, contrôles timeout et erreurs HAL, traces lisibles, instructions de build/flash et configuration du STLINK de la seconde carte à identifier. Éviter d'intégrer les services FRAM, SD, Modbus, persistance et autres composants du TR2.
2. Préparer deux exemples officiels reproductibles : polling WHO_AM_I/reset/BDU/XYZ, puis FIFO stream continu avec lectures/tags/statuts. Respecter séquences ST, distinguer valeurs et modes d'origine des adaptations de plateforme. Conserver un profil « ST strict » et un profil « comparaison TR2 » seulement si nécessaire, sans masquer les différences. Comparer ligne à ligne notre code I/J aux APIs et séquences ST ; matrice conforme/différent/non établi, notamment reset, IF_INC, BDU, CTRL1, FIFO_CTRL3/4, SPI 4 fils, commande R/W et auto-increment, CS et burst.
3. Compiler et tester hors matériel, produire manifestes sources/hashes, scripts de diagnostic et capture brute. Préparer un protocole d'essais progressif avec critères WHO, XYZ non figés, FIFO, erreur et arrêt, comparaison banc isolé vs bus TR2. Ne pas prétendre qualifier le breakout sans exécution physique.
4. Ne flasher la seconde NUCLEO qu'après confirmation du câblage et identification certaine du STLINK/UID de la **seconde** carte, sans toucher à la carte TR2 principale. En cas d'incertitude, terminer en READY_FOR_PHYSICAL avec consignes précises, pas BLOCKED logiciel. Si carte et câblage confirmés dans une étape ultérieure, flash/debug autorisés selon AGENTS.md avec garde-fous, sauvegarde initiale de la carte cible, verify/readback et aucune opération destructive inhabituelle. Ne jamais utiliser l'identité STLINK historique du TR2 comme identité du banc.
5. Publier code, instructions de câblage vérifiées, audit, tests, statut honnête et prochain protocole dans DERNIER_RAPPORT.md et état courant. Commit/push main et vérification distante. Préserver le firmware permanent M014-H sur la première NUCLEO ; ne modifier ni mapping V1, ni autres médias ou câblage TR2.

## Critères d'acceptation
Projet officiel ST portable compilé, sources tracées et comparées, pinout sourcé, procédure de test claire. Les essais physiques et la preuve du fonctionnement du breakout sont distincts et ne peuvent être déclarés DONE avant montage/confirmation et acquisition réelle. Développement logiciel autonome sans demander validations répétitives ; seule la connexion physique et les décisions matérielles reviennent à l'utilisateur.

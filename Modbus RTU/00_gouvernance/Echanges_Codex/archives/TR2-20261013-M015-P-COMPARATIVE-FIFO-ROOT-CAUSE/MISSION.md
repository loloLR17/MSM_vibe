---
mission_id: TR2-20261013-M015-P-COMPARATIVE-FIFO-ROOT-CAUSE
status: READY
base_ref: main
base_sha: c3e85afbd32f38f07687d2f12cf07a96fc2ed6ee
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-P — Comparaison forensique du FIFO XYZ réussi (M015-I) et du banc ST en échec (M015-O)

## Objectif et décision
L'utilisateur demande de poursuivre sans attendre. **La priorité n'est plus de multiplier les variantes du portage ST**, mais d'expliquer par des preuves la divergence entre :
- M015-I : sur le TR2 principal, **256 échantillons XYZ FIFO réellement obtenus**, deux bursts de 128, rapport et traces disponibles ;
- M015-O : sur la seconde NUCLEO + STEVAL-MKI208V1K, fonction officielle ST exécutée, 258 timestamps, 0 XYZ, modes SPI 0 et 3 identiques.

La contre-expertise externe propose de vérifier les octets bruts, le décodage des tags, les écritures/rerelectures de registres et la croissance FIFO. Ne pas adopter ses hypothèses sans confrontation aux preuves : M015-N/O ont déjà fourni les tags bruts/parités, valeurs 07..0A = 00/01/0A/86, CTRL1_XL=AC, et les lectures SPI complètes. **Ne pas refaire inutilement ces tests.**

## Préparation gouvernance
Lire intégralement AGENTS.md, protocole V2, ETAT_COURANT_TR2.md, état réel de main/remote, git status, worktrees et stashes ; préserver tous travaux. Archiver exactement mission M015-O et DERNIER_RAPPORT M015-O selon protocole. Identifier SHA exact de chaque artefact et code testé. Aucune supposition fondée sur une ancienne conversation.

## Phase A — Audit documentaire et binaire, sans matériel
1. Retrouver dans le dépôt et preuves archivées **la source exacte M015-I (commit 6c504a9d4035c2922d186a03350963bdac33379b et rapport de qualification physique M015-I)**, ses écritures SPI, initialisation du capteur, séquences FIFO, commandes de lecture, tags bruts, registres pré/post et éventuelles particularités matérielles. Distinguer la réussite physique documentée des hypothèses.
2. Retrouver la source M015-O (code évalué 906de1ad85f6b3d7d6737bc699f29a482590614d, rapport d46a3d24..., provenance ST, driver f83e74... distinct du gitlink d995...) et ses captures brutes. Conserver les références officielles et différences de versions.
3. Comparer **registre par registre et transaction par transaction** : reset, BOOT, BDU, IF_INC, CTRL1_XL, CTRL3_C, CTRL10_C, FIFO_CTRL1..4, ordre et délais d'écriture, read-modify-write, FIFO_STATUS1/2, FIFO_DATA_OUT_TAG 0x78 et lecture 7 octets, SPI CS/auto-increment/burst, décodage tag ((raw>>3)&0x1f), échantillons XYZ vs timestamps et éventuelles reconfigurations pendant les bursts. Vérifier les registres à partir de la doc officielle ST, pas d'hypothèses externes.
4. Examiner en priorité l'écart observé M015-J (FIFO_CTRL3 relu 0x14 contre 0x0A et CTRL4 0x0C contre 0x06 après vidange) sans présumer sa causalité. Examiner aussi **le fait que M015-I réussit sur une autre carte et un autre module** : ne pas confondre différence logicielle et différence matérielle. Si un dump M015-I est inaccessible, consigner précisément la lacune.
5. Produire un tableau d'équivalence/différences **avec chemins, lignes, commits, extraits de transactions et force de preuve**, et une matrice hypothèse ↔ observation discriminante. Vérifier si le prétendu problème de tag shift >>2 est déjà réfuté par raw bytes (ne pas répéter une assertion non contrôlée). Ne pas conclure « capteur défectueux » ni « SPI mauvais » sans preuve.
6. Vérifier les sources TR2 déjà fonctionnelles : distinguer lecture directe XYZ de la qualification FIFO XYZ M015-I. Aucun flash du TR2 principal, aucune lecture intrusive du principal, aucun échange de capteur, FRAM/SD/Modbus inchangés.

## Phase B — Diagnostic causal minimal
Sur la base du comparatif A, choisir **une seule** hypothèse principale justifiée, avec prédiction mesurable et un test de contrôle permettant de la réfuter. Préférer l'exploitation de traces existantes, puis des tests hôte/mock. Si une différence logicielle précise est identifiée, préparer sur le **second banc seulement** un portage minimal de la séquence FIFO qui avait réussi M015-I, en conservant le témoin et en documentant chaque différence par rapport à ST ; ne pas toucher au firmware TR2 principal.

**Autorisation matérielle explicite :** `allow_flash: true` et `allow_debug: true` pour la seconde NUCLEO exclusivement, uniquement si l'audit A révèle une hypothèse discriminante testable et si la chaîne qualifiée est respectée. Ne pas lancer de campagne physique par simple épuisement des hypothèses. Avant chaque flash : SN STLINK `0032001F3235510C37333439`, UID `001B00463842501820333157` ; refuser la première NUCLEO SN `003500463235510B37333439` et UID `004A003D3842501820333157`. Image et empreinte vérifiées, flash normal SWD/verify/readback, sans backup/restauration 2 MiB sur la carte jetable, sans mass erase/Option Bytes/RDP. Préserver trace et cleanup même si le test échoue. Au plus **un essai physique causal et son témoin de contrôle**, sans réglages heuristiques successifs. Pas de manipulation physique sans utilisateur.

Si le comparatif ne révèle aucune hypothèse réellement discriminante, **s'arrêter après phase A** et publier BLOCKED avec les pièces manquantes ou la mesure indépendante réellement nécessaire (analyseur logique, tension au module, etc.), sans fabriquer de faux succès.

## Livrables et critères
- Rapport de comparaison M015-I vs M015-O exploitable par ChatGPT et par un expert tiers (Claude), avec tableau détaillé, preuves brutes, SHAs, sources ST, explication du diagnostic de tag, registres et causalité.
- Code/patch/tests ciblés uniquement si utiles, builds proportionnés ; pas de modification du driver vendor ni du firmware gelé.
- Statut DONE uniquement si la cause est **établie** par preuves discriminantes et, pour une prétention de résolution physique, FIFO XYZ effectivement observé sur seconde carte ; sinon BLOCKED / diagnostic partiel clairement identifié.
- Mettre à jour ETAT_COURANT_TR2.md, roadmap pertinente, archives et `DERNIER_RAPPORT.md`, commits/push sur main, vérifier relecture distante des livrables et état Git. Les validations ne doivent jamais être déclarées sans logs effectifs.

Autonomie permanente développement, commit, push, flash et debug appliquée sans solliciter d'approbations répétitives ; interventions physiques et décisions structurantes seulement à l'utilisateur.

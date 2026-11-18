# Archivage de la mission TR2-20261117-SHIELD-DUAL-BRINGUP (révision 2)

Le 18/11/2026, après l'audit E-01 livré au commit 2a34b52a2bee439ee334fe367eef640e14871a3d et la revue indépendante publiée au commit ac8c6fc82829d664882c60e1a10c73ba7e3aa0e1, l'utilisateur donne une nouvelle priorité explicite : qualification physique exclusive du circuit RS-485 Click. Cette mission précédente reste **BLOCKED pour son bring-up général** et **n'est pas déclarée DONE**. Son historique, ses résultats FRAM/SD et ses réserves SPI3 sont conservés. Ce contrat historique est archivé pour empêcher de relancer un deuxième pilote actif ; la mission RS485 est indépendante, sans annuler les NO-GO E-01 concernant SPI3 et les autres interfaces.

---

---
mission_id: TR2-20261117-SHIELD-DUAL-BRINGUP
status: READY
revision: 2
resume_reason: USER_APPROVED_E01_FULL_ELECTRICAL_AUDIT_2026-11-17
base_ref: main
base_sha: f019dfbaa4e2cf9cf181031d0534013b89d989e1
created_at_utc: 2026-11-17T17:53:00Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# TR2 — Qualification électrique et fonctionnelle progressive des deux ensembles assemblés

## Faits utilisateur et périmètre
Deux NUCLEO STM32U575 avec shields TR2 assemblés électroniquement, sans boîtiers imprimés ni entretoises mécaniques. Les modules IIS3DWB/MKI, FRAM, microSD Adafruit et RS485 Isolator 2 Click peuvent avoir été permutés entre ensembles 001 et 002 : **ne jamais inférer l'identité des périphériques depuis l'identité de la Nucleo**. Une seule microSD 32 Go de banc, historiquement 002, est disponible ; utilisateur déclare sa sauvegarde faite et autorise la suppression des données de **cette carte explicitement identifiée uniquement**. Ne pas toucher aux autres cartes, 512 Go ou données et FRAM historiques. Tester un seul TR2 à la fois, l'autre hors tension/débranché. Samtec J5 peut être absent : PG4/RTU_DE non raccordé ; ne pas déclarer de transmission Modbus validée si le pilotage DE requis manque. Le PLA de Morvan n'est pas encore reçu. Ne pas entreprendre d'essai endurance ou mécanique sous contrainte.

## Mission
Codex doit inspecter l'état réel T1000, les identités des deux Nucleo, l'architecture shield V2.3, la configuration officielle du Click MIKROE-3863 HW réellement reçu (y compris jumpers half-duplex J7/J8, JP1 alimentation 3V3, DE/RE, TERM, BIAS), les preuves de qualification existantes et l'état des drivers/outils. Produire une procédure minimale et exécuter de manière autonome **les essais possibles** successivement sur 001 puis 002, avec mesures et traces brutes de chaque périphérique, sans présupposer la permutation des modules :
1. Gate montage hors tension avant alimentation : inspection guidée utilisateur des éventuels courts-circuits/continuités +3V3/GND, orientation C2, absence de bridage/contrainte mécanique des cartes non fixées, soudures, positions de cavaliers, câblage et masse isolée ; stop si anomalie.
2. Identification fraîche par UID STM32, ST-LINK, firmware/empreinte, contrôle alimentation 3V3 et sources ; inventaire distinct de chaque assemblage et périphériques effectivement identifiables.
3. Tests non destructifs IIS3DWB (WHO_AM_I, configuration, axes/échantillons plausibles), FRAM (identification si supportée, test sur zone scratch explicitement libre ou méthode non destructive avec sauvegarde/restore byte exact contrôlée et sans compromettre les données de référence), SD (CID/CSD/capacité, FAT, fichier temporaire unique / écriture-lecture-CRC et suppression seulement après identification sûre de la 32 Go et absence d'autre stockage monté), lecture des interfaces et supervision locale pertinentes.
4. RS485 isolé : confirmer au schéma et à l'inscription réelle du Click la correspondance A/B récepteur, Y/Z émetteur, pontage half-duplex fabricant, GND2 séparé, termination/bias selon câblage réel et polarité du convertisseur USB non présumée ; confirmer rôle de J5/DE absent, tester réception passive si faisable, puis émission/échange Modbus uniquement après commande DE réellement établie et bus protégé ; ne jamais contourner absence de DE avec une affirmation de PASS.
5. Reproduire sur le second TR2 avec la même microSD transférée par l'utilisateur hors tension ; documenter pour chaque composant PASS/FAIL/BLOCKED/NON_TESTÉ et limites, incluant l'effet du câblage/ensemble choisi.

## Garde-fous
Conserver les spécifications Modbus V1 et le firmware/contrôles gelés ; aucune refonte, pas de formatage automatique de masse, pas d'effacement de FRAM ou SD hors fichier scratch, pas de modification d'identité/Option Bytes/security/boot. L'autorisation d'effacer la microSD 32Go identifiée n'est pas une autorisation d'effacer une autre carte ; n'utiliser un formatage de cette seule 32Go que si indispensable, en démontrant explicitement sa sélection, sa sauvegarde déclarée et la raison. Standard flash/debug autorisés dans le cadre des chaînes matériellement qualifiées AGENTS.md ; vérifier UID/programme/image/readback et récupérabilité, préserver média et autres fonctions. Escalader les branchements, insertions/retraits et mesures physiques à l'utilisateur, pas le travail Git/logiciel. Ne pas alimenter le blindage tant que les réserves mécaniques essentielles ne sont pas levées. Pas de contention ST-LINK/COM/WSL/Windows ni de double maître Modbus.

## Critères et livrables
Rapport DERNIER_RAPPORT.md, résultats séparés par ensemble avec numéros UID et association constatée de périphériques (ne pas inventer numéro de série module si non détectable), configuration jumpers et traces horodatées, versions/SHA build/flash et statuts physiques, identification de la carte 32 Go, résultat FRAM vérifiable sans perte des acquis, tests RS485 TX/RX et Modbus conditionnels. Adapter les outils/tests ciblés sans régression et publier commits sur main selon gouvernance ; laisser en IN_PROGRESS ou BLOCKED avec preuve et étapes précises lorsqu'une action physique utilisateur est requise. Ne pas déclarer DONE tant que le contrat complet n'est pas démontré et ne pas revendiquer qualification industrielle/endurance.

## Avenant de reprise E-01 — décision utilisateur du 17/11/2026 (même mission, non remplacement)

Le rapport Codex de cette mission a constaté `BLOCKED` (erreur SPI3 structurelle) et doit rester conservé, sans être réinterprété comme `DONE`. L'utilisateur demande explicitement de reprendre **l'audit électrique exhaustif de toutes les interfaces** du shield V2.3, avant toute décision de reprise ou refabrication. Cet avenant maintient le même `mission_id`; `READY` désigne uniquement la **tranche d'audit reprise**, et n'annule aucun blocage physique antérieur. Aucune nouvelle mission concurrente ni aucune action sous tension sur les shields.

### Objet immédiat et portée

Appliquer impérativement GATE E-01 dans `AGENTS.md` et `GOUVERNANCE_CHATGPT_CODEX.md`. Faire le **contrôle indépendant intégral des signaux Nucleo→shield→breakout** à partir des sources **constructeurs** et des révisions réelles, en s'interdisant de prendre la matrice interne historique ou l'empreinte KiCad comme vérité. Comparer les pinouts natifs ST NUCLEO C05 et MKIGIBV2/MKI208V1, Adafruit4718 FRAM, Adafruit4682 microSD, MikroE RS485 Isolator 2 Click MIKROE3863 **HW v1.02 effectivement utilisé**, incluant v1.01/v1.02 non confondues. Auditer 100 % des broches de J1..J9 et des rails/TP/pullups/passifs intéressants, y compris NC, signaux +3V3/GND/GND2, /RE/DE/J2 du Click, J7/J8, VCCSEL, UART TX/RX, séparation galvanique, et tout risque de court-circuit ou contention.

**Preuves attendues** : matrice indépendante constructeur `source/version/url/sha` → pin physique breakout → net souhaité MCU → KiCad schéma → KiCad PCB → CAM livré ; pour chaque net verdict individuel PASS/FAIL/INCONNU avec preuve et conséquence. Contrôler sens/miroir des connecteurs, fiche de fabrication effective et archives, et reconstituer chronologiquement **où et quand** la mauvaise affectation MKIGIBV2 a été introduite, malgré les précédentes acquisitions sur fils ; chercher historique des approbations demandées sur Click et distinguer vérification de géométrie, vérification électrique et simple parité interne. Signaler toute autre divergence sans la réparer aveuglément.

**Coût/récupération** : inventorier les composants effectivement soudés et achetés selon BOM et historique de commande ; séparer composants réutilisables sans dessoudage, potentiellement récupérables (risques thermiques/mécaniques), consommables et éléments non commandés. Chiffrer seulement si prix actuel sourcé ; sinon indiquer quantité et coût inconnu. Étudier comparativement réparation des deux cartes existantes, interposeur, ou nouvelle fabrication, avec impacts sur mécanique V2.7, temps, capacités de production, risques, sans détruire ni désouder. Aucun nouvel achat/commande et aucune réparation physique décidés par Codex.

**Livrables** : audit E-01, RCA documentée de l'erreur SPI3, inventaire financier/réemploi prudent, préconisation corrective non destructive sur fichiers de **variante distincte** seulement si nécessaire aux preuves ; conserver V2.3 original, ne pas changer CAM définitif ni lancer production ; rapport avec statuts factuels et références de commits. ChatGPT réalisera sa propre contre-lecture des sources avant décision structurante. Pas de flash/debug/essai sous tension requis pour cet audit documentaire même si les permissions de la mission restent `true` pour la reprise complète ; les garder inutilisées tant que le gate physique reste bloqué. Pas de relance de tests déjà acquis.

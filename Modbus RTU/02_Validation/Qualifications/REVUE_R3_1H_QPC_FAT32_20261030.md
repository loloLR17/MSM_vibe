# MSM TR2 — Revue A-01 de la campagne R3-1H indépendante QPC + FAT32

**30 octobre 2026 — ChatGPT — Verdict : ACCEPTÉ dans le périmètre expérimental R3-1H.**

**Porte démontrée :** au moins **3 601,0368139 secondes** mesurées **sur une référence QPC Windows indépendante du STM32** et situées **à l'intérieur d'une même capture active** TR2-002 ; le fichier FAT32 associé à cette capture est récupéré physiquement par le lecteur T1000 et les **63 422 658 octets utiles** sont conformes au digest producteur antérieur aux écritures. **Ce n'est pas un étalonnage SI ni une qualification 8h, coupure, V1 industrielle, FRAM ou Modbus.**

## Sources, identité du code et revue indépendante

- Consultation sur **`main`** de `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, protocole, contrat `TR2-20261029-R3-1H-FAT32-ENDURANCE` (`READY` historique), dernier rapport `DONE`, roadmap, code des témoins, archive du rapport et manifestes ; **HEAD consulté `5e761939f55aee05ecff2af0758df101bed40273`**. Rapport `DERNIER_RAPPORT.md` blob `96bdefdfaa35c303b6f255ec959de7579265acfd` identique octet pour octet à `archives/TR2-20261029-R3-1H-FAT32-ENDURANCE/RAPPORT.md`. Archive du contrat blob `744471329ce919841f094bee522e3dee88a11778`, identique à l'actif lors de la revue.
- **SHA de code flashé et réellement exécuté selon le rapport : `24aaf06a4f975e54b8a6199d6c625fab612cf005`** (CMake `R3_QPC_WITNESS=ON,R3_ENDURANCE_1H=ON`), distinct des commits documentaires de preuve `6d42a8744ab8f6264ed5b9fbb69b00c27261347d` et de clôture `5e761939f55aee05ecff2af0758df101bed40273`. SHA BIN `6cb4c7f93db96bca888a3950e99345acd7deedb1cd7e23d75296cdcac193f7d8` (55 324 B), ELF `a022b8384979bc9cabb9543bb39e6eea3c8ab0eabae44bfed51ae8322ed59e39`. Builds et lecture de tous les octets BIN après flash rapportés PASS (pas une relecture de tous les 2 MiB Flash).
- Banc **TR2-002 uniquement**, ST-LINK `0032001F3235510C37333439`, UID `001B0046/38425018/20333157`, IIS3DWB `WHO=0x7B`, SPI3 **8 MHz**, microSD SPI2 **4 MHz**, CID test `0353445344333247858bf0923c01927b`, FatFs FAT32 ; PC T1000 lecteur Realtek. Protocole de déclenchement RAM ponctuel ; **GDB détaché pendant l'acquisition et arrêt/flush gérés par le STM32**. `TR2-001`, FRAM, COM4, service Windows et carte de production non accédés.
- Sources inspectées directement : `experiments/r3_time_reference/src/{witness_uart.c,witness_frame.c}`, `tools/{witness_capture.ps1,analyze_reference.py}`, `experiments/r3_durable_fat32/src/{main.c,fat_volume.c}`. Dans `poll`, un paquet `QPC2` actif est émis uniquement si `fifo.running`, `writer_enabled`, `keep`, données acceptées et sans défaut ; un octet UART au plus par service, sans attente bloquante et sans START/STOP côté Windows. Le paquet porte `UID,sequence,us,flags,session,accepted,CRC32`; le contrôleur exige deux paquets actifs de même session, séquences successives, sources et temps croissants.
- Preuve QPC décompressée et relue indépendamment par ChatGPT depuis la représentation Base64 de l'objet GitHub `qpc-final/024-qpc-long-analysis.json.zlib` : intervalle interne **3601,0368139 s**, session `8444552830526285`, A séq40/5928XYZ, B séq41/95810520XYZ, terminal séq78/105704424XYZ et état final. La règle d'intervalle **`receive_A < send_B`** (appel Windows QPC à 10 MHz) borne réellement une durée **entièrement comprise entre START et STOP**, sans prendre l'UTC Windows comme étalon. Cette vérification est **une seconde lecture d'un fichier dérivé de preuve** ; la reconstruction brute des trames et la copie physique sont documentées par Codex, pas réexécutées par ChatGPT.
- Preuves référencées : `Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261029-R3-1H-FAT32-ENDURANCE/qpc-final/BUNDLE.json` (manifestes SHA bruts et compressés), `publication-receipt/BUNDLE.json`, reçus de la livraison technique `6d42a874...`. Le rapport Codex indique **509 fichiers comparés et 433 artefacts décompressés/re-hachés**, plus reconstruction intégrale CRC/indices/temps/footer/SHA des sections de données obtenues depuis GitHub. Ces nombres proviennent de la vérification Codex, **non d'une répétition indépendante par ChatGPT**.

## Résultats acceptés et arithmétique contrôlée

| Élément | Campagne Q1N0005 |
|---|---:|
| Durée inférieure QPC, entre deux témoins actifs | **3601,0368139 secondes (plus d'une heure)** |
| Durée de garde STM32 nominale, distincte du QPC | **4000,000029 secondes MCU** |
| XYZ natifs acceptés | **105 704 424** |
| XYZ sortis par FIR Q15 /10 | **10 570 443** |
| Octets XYZ utiles | **63 422 658** |
| Records 512B acquittés | **135 519** |
| Taille du fichier FAT32 original | **75 497 472 octets (72 MiB)** |
| SHA256 utiles, producteur amont/lecture PC concordants | `d101531be87f3a921b30ef78cde3071ba79f7304c453a3fa36e6393f2c3dad5d` |
| SHA256 fichier entier original sur carte et copie | `c6098f96bf1fc8f76d2645cd5b6c97223217ba35138cd666bc6358ef4c08575e` |

Contre-calculs : `ceil(105704424/10)=10570443`, `10570443×6=63422658` et `135519×512=69385728` octets de records. `75 497 472-69 385 728-1 024=6 110 720` octets de zone préallouée non utilisée. Cette zone **n'est ni de la mesure XYZ ni présente dans les artefacts GitHub** ; les originaux complets restent conservés sur T1000, leurs hashes entiers sont rapportés.

**Checks exécutés par Codex selon preuves** : gate court `Q1N0003.TR2` (5 260 sorties/31 560B, 131 072B) puis un seul long justifié `Q1N0005.TR2`, tous les CRC header/135519 records/footer et SHA producteur-vs-lecteur, identité session UART/PC et compte natif, anciennes huit campagnes RUN/R2N/H1N et TR2INFO inchangés, préfixes128 comparés aux dumps RAM et103 valeurs FIR recalculées indépendamment par run. **Ne pas affirmer que les 105 M XYZ bruts ont tous été recalculés scientifiquement**.

**Marges observées, non WCET universel** : FIFO max50 mots, zéro OVR ; file max **10 240/65 536B**, zéro overflow ; work max683,325 µs (`<1000`), service1411,219 µs (`<2500`), gap1668 µs (`<5000`), quantum SD430,419 µs. Busy max secteur159675 µs, groupe356137 µs, absorbés lors de cette campagne ; longues grappes arbitraires toujours non garanties. 4 160 636 services et 39 787 contrôles temporels. Pile observée profondeur1768B/plancher2328B dans la zone peinte4KiB, pas une analyse exhaustive de pile/interruptions.

**Temps :** le rapport documente le franchissement effectif du seuil `2^32 µs` dans les timestamps source u64 du fichier, ainsi que la distinction entre cadence native capteur, temps MCU nominal et référence QPC. La mesure QPC a une granularité10MHz mais n'est **pas un certificat SI**, et le QPC externe n'est **pas un composant de commande** du STM32. Le `/10` reste expérimental : aucune modification de la norme historique V1-05 2000Hz, aucune métrologie ±8g physique validée. L'ancienne capture H1N/E4 sans référence externe reste seulement qualifiée en intégrité et **n'est pas rétroqualifiée** par ce nouvel essai.

## Sécurité, limites et décision de la suite

Le diagnostic Defender `Injuke.C!MTB` appliqué à une commande `Parser.ParseInput` de validation syntaxique/CRC est documenté séparément dans `ALERTE_DEFENDER_QPC_T1000_20261030.md`; le rapport Codex indique protections Windows maintenues, aucun nouvel événement Defender corrélé au témoin physique. Aucune exclusion, restauration de menace ou désactivation. **Pas de conclusion d'innocuité générale du T1000.**

**État final déclaré et consigné :** TR2-002 **hors tension**, microSD dans le lecteur PC après transfert confirmé, ancien firmware QPC en Flash, boot WAIT, COM5/GDB fermés. Toute reprise physique nécessitera geste utilisateur au bon moment après arrêt sûr. Pas de nouvelle capture implicite, ni écrasement d'historiques.

**Qualification A-01 : ACCEPTÉE** pour **enregistrement FAT32 récupérable d'une campagne continue dont un intervalle indépendant QPC de plus de 3600s est prouvé**. Cette acceptation n'implique **ni durée exactement 3600 s, ni UTC fiable, ni SI étalonné, ni endurance 8h, ni essais de coupure/fichier orphelin, ni robustesse à toute pause SD, ni coexistence FRAM/RS485/Modbus, ni conformité finale produit V1**.

**Pas de tag annoté `tr2/Mnnn` :** R3-1H n'est pas un jalon produit Mnnn individuel terminé ; la porte R3 produit à 8h et la coexistence des sous-systèmes restent à qualifier. **Autorise la publication de la mission R3-8H distincte**, avec firmware entièrement autonome et sans aucune session Codex pendant les huit heures, compteur et format 64 bits versionnés et preuve PC indépendante.

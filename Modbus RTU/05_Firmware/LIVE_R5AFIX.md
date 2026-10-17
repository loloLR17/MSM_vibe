# R5-AFIX — remise en conformité des offsets B3

Mission `TR2-20261017-R5AFIX-B3-WIRE-ORDER`, référence contractuelle e100770, HEAD initial5103ea4. Les archives R5-A BLOCKED et les captures historiques sont préservées.

## Correction et oracles

Le projecteur LIVE écrivait la sévérité en3002 et les alarmes en3003. Deux destinations sont corrigées : **3002=ALARM_FLAGS,3003=SEVERITY_GLOBAL**. Les expressions, masques, latch, dépassements et sévérités restent identiques. R0-ALARM ne produit pas la sévérité critique5 automatiquement ; les cas LIVE testés sont0..4, sans modification du domaine V1 hérité0..5.

Le nouvel oracle vérifie les mots et les octets ADU aux positions littérales3002/3003, sans déduire les attentes du projecteur. Neuf états : boot invalide, attente, normalUTC, frais sansUTC, dépassement, alarme+latch, HELD aprèsSTOP, latch sansalarmeactive, erreurcalcul. Les attentes sontrespectivement alarmes/sévérité0/0,0/0,0/1,0/2,0xF0/3,0x1FF/4,0x1FF/0,0x100/2,0x100/0. L'oracle échoue sur le code initial et passe après correction. Une assertion antérieure testait elle aussi le mauvais offset : elle est corrigée vers3003, sans supprimer son contrôle d'alarme.

La revue ciblée du mapping et des projecteurs n'a pas trouvé d'autre inversion adjacente démontrée : types/MSW-LSW/offsets4..29, dominant30, dépassements31..34, latch35, compteurs36..39 et réservés40..47 restent inchangés. `modbus_project_b3` est conservé ; la spécialisationLIVE est le lieu du défaut. DécodeurPC normatif inchangé, aucun échange de mots pour tolérer les anciennes trames.

Les neuf ADU exportés par le test C constituent une **fixture de simulation**, pas une capture matérielle. Un oracleC# indépendant vérifie alarmes/sévérité/qualité/UTC et leur persistance SQLite : neuf observations sansquarantaine, cinqvalides/fraîches. Les2068anciensB3 R4-U/V restent refusés par les testsR5-A. Tests hôte/sanitizers/ARM/.NET : voir logs effectifs du rapport.

## Plan physique borné

Préflight frais11:36:20UTC : TR2-001 SN003500463235510B37333439, UID004A003D/38425018/20333157 ; M016Flash2Mio c5bc344d…f5b26 et FRAM262144octets70e273…73587 exactes aux référencesR4-V, capteurWHO7B/ODRoff/FIFObypass/TSoff. COM4FTDI0403:6001/BG04KSSW retrouvé surT1000, aucunmaître natif concurrent répertorié ; COM3STLINK exclu. Sauvegardes complètes fraîches vérifiées, pas de reset/flash/écriturepersistante àcepréflight.

Un simple B3 readonly initial a alarmes/sévérité0/0 et ne distingue pas les deuxoffsets. Pour une preuve réelle avec valeurs et qualité autoritatives, le contrat autorise un nouveau START/STOP R2 : **une seule paire5/10STARTrun10 et5/11STOP**, après les gardes exactes FRAM/journal62/root75/epoch5/run9/config3gen3. Aucun retry/collision/refus supplémentaire ni réutilisation d'identité ; aucune retraite/RENEW. Le modèle horsligne des métadonnées emploie le vrai store et prédit journal70/root83/run10 ; il ne prouve pas l'effet matériel.

Le probe NON-PRODUCTION distinct réutilise le harness qualifiéR4-U et le pacing250µsR4-V aprèsfindrain. Il attend untrigger explicite, garde le propriétaire FIFO unique, les budgets1000/2500/5000µs et le STOP sûr ; il n'invente aucune autorité. Le maître natifCOM4exclusif1152008E1 litd'abordB0/B3, vérifieCRC/longueur/offsets/UTC puis autorise la seulepaire. Pas de GDB pendant transfert ; checkpoints avantSTART et aprèsSTOP. Le résultat de la campagne n'est pas anticipé par cette préparation.

Après l'essai : auditFRAM entière contre modèle et partitions, comparaisonRMS/crêtes aux4096bruts si collectés, décodagePC desnouveauxADU, puis restaurationM016 parpagesnormales et relectureFlash2Mio/FRAMentière. AucunrollbackFRAM, SD, HP, OptionBytes/RDP/masserase ouproduitLIVEpersistera. R5-B/maîtrePCversSQLite/Webphysique etqualificationindépendanteChatGPT restent séparés.


## Résultat physique et fermeture — 17 octobre 2026 UTC

Le plan ci-dessus a été exécuté une seule fois. Code firmware `29a6f4ecf83c97499055b58fc73d631763cf5813`, flash après publication/relecture du checkpoint23c6bd3. BIN147640octets SHA d50fe6bbdde1b5127ee9cd2c5f3ffcbaae5045453153df71eb6e793a2b13f99b ; ELF9b10196a43d08ee75fd28df68e7a77e12b66f599d47494c411a7d737aa2da040. Identité et readback de l'image vérifiés avant le trigger. Les règles R2, le runtime FIFO et le décodeur PC n'ont pas été modifiés.

| Observation | Résultat réel |
|---|---|
| Commandes nouvelles |START5/10/run10 etSTOP5/11 seulement, terminaux4/0 ; aucunretry/collision/RENEW supplémentaire|
| RS-485 COM4 |1095ADU CRCexactes ;563B3 dont462V/F et51HELD|
| Premier B3 V/F |11:40:02.6886323UTC, statut4/flags1215, **3002=240/0x00F0,3003=3**, séquence1, UTCabsente|
| Dernier HELD |statut1/flags5819, **3002=240,3003=0**, séquence34, FRESH0|
| Acquisition |1784856XYZ/223107TS, FIFOmax70, erreursFIFO/UART/drop0|
| Budgets inchangés |work809/service2116/gap2344µs sous1000/2500/5000|
| Oracle première4096XYZ |RMS global/X/Y/Z954/953/10/27mg ; peaks996/996/34/73mg, exacts aux sortiesMCU|
| Terminaux mesurés par maître |START39,893016s ;STOP39,967197s, pas une nouvelle borne industrielle|
| FRAM |entière exacte au modèle4e7fabde5da1100cf9ae8faf262ea079727856826bb2829c1e99b68ad63b84bd ; journal70/root83/epoch5/run10/config3gen3/effect45phase3|

Les419408octets écrits/8commits sont limités à LIVE ; historique et tail byte-identiques. Aucune identité réutilisée et aucun snapshot FRAM restauré. Les valeurs physiques ne sont pas une calibration métrologique. Les alarmes actives, latch et erreur restent couverts par les neuf oracles hôte ; ils n'ont pas été provoqués artificiellement sur la carte. Les maxima observés ne prouvent pas le WCET IRQ/HAL ou l'endurance.

**M016 restauré11:41:55UTC**,19pagesnormales/155648octets ; relectureFlash2097152octets exacte c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26. FRAM262144octets relue après restauration exactement4e7f…84bd. WHO7B/ODRoff/FIFObypass/TSoff vérifiés ensuite, CommissioningReadOnly WAIT, GDBdétaché/COM4fermé. Aucun SD/HP/câblage/OptionBytes/RDP/masserase/rollback.

## Acceptation PC et limites conservées

Le codePC de production est inchangé. Le nouveau test relit les563B3 authentiques avec le décodeur strict R5-A et persiste563observations SQLite sans rejet ; V/F/HELD, UTCabsente et contexte run inconnu restent respectés. Les headersB4/B6 nécessaires au replay restent simulés et explicitement étiquetés, puisqu'ils ne figurent pas dans ces ADU ; les B0/B3 sont réellement capturés. Ce contrôle horsligne n'est pas R5-B ni une chaînePC→SQLite→Web physique en tempsréel.

146testsfirmware complets/cross-build canoniquePASS96,02s, BINcanoniqueV1 inchangé85b1…c0f5 ;4suitesO2ASan/UBSanPASS8,80s ;202testsServicePASS avant physique (dontanciensnégatifs etfixturepositive), puis2testsR5AFIXPASS après ajout du replayphysique. Aucun ancientest supprimé ou affaibli ; aucun test réussi rejoué après ces acquisitions sans modification concernée.

Les2068anciensB3 restent des preuves négatives, le rapport R5-A BLOCKED n'est pas réécrit DONE. R5-AFIX lève le défaut identifié dans son périmètre. R5-B exige toujours raccordementPC COM4 unique, identification réelle du firmware, composition B1/B2/B7/capacités complètes et preuveSQLite/Web. Aucun produitLIVEcomplet, tag ou GO industriel/R5/R6 n'est donné ici ; contrôle indépendantChatGPT requis.

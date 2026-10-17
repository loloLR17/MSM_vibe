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

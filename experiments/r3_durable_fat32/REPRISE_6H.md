# État après récupération des6h — données disponibles, pas de relance

Récupération effectuée le31/10/2026, missionTR2-20261030-R3-8H-FAT32-AUTONOME / instruction utilisateur6h exploratoires. Relire main courant et DERNIER_RAPPORT.md avant toute suite.

## Fichiers originaux complets préservés

Répertoire `/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/` :

- `E6N0005.TR2`,671088640B ; SHAentier `91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5`.
- `E6N0003.TR2`,131072B ; SHAentier `325c9e6093975ed9bfe5340a3bd07c20c677ca498f5fb52c439ce07c0dec8c67`.
- `RECOVERY_RESULTS.json`, `COPIES.json`, `HISTORICAL_HASHES.json`, métadonnées natives duvolume.

Origine carteT1000nativeRealtek/FAT32/capacité31914983424B, originauxencoresurSD ; copiesexclusives, SHAcarte=copie. Lecteurstrictv3 inchangé **COMPLETE pour les deux**. Long :750669records/58552136XYZ/351312816Butiles SHA `5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41`, égal aufooter ; rails0/clips0,11warmup. Session8444550407367521/585521360natifs identiques au terminalQPC1284, duréeintérieure21601,0193586s. Sourcecode c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e/BINb704…5330, v3int16/FIR107 /10/DCconservée/±8glogique/244µgcompte nominal. TempsMCU etinterpolationintra-record, pasUTC deséchantillons ni cadence2000HzV1 qualifiée.

## Réserve indépendante après coupure

L'utilisateur a confirmé une coupure pour déplacer lebanc versl'atelier. Au retourGDB : WAIT/run0/compteurs0/digestRAM32B0. Le digest amont indépendant du long et ses statistiques latence/file/pile n'ont pas été récupérés. **Le digest dufooter n'est pas une référenceRAM externe indépendante** ; comparaisonperdue, qualificationcomplèteBLOCKED même si récupérationdesdonnéesachevée. Le gatecourt dispose de sa référenceRAM antérieure et comparaisonPASS.

Les snapshots `post-reset-state/current-*` décrivent le bootaprèscoupure, jamaislescompteursfinaux. `six-long-data/` est resté vide lorsque la garde de recovery a refusé l'étatreset. **Ne pas relancerrecover_terminal.py ni créer une acquisition pour remplacer la preuve perdue.** Aucun reset/flash/START/Modbus n'a été réalisé pendant la récupération.

## État physique et suite

Dernièreconfirmationuser : microSD danslecteurPC, TR2-002 hors tension/CN15off. GDBdétaché ; TR2-001/FRAM/COM4intouchés. Treizeanciensfichiers etinformation re-hachésinchangés. Donnéescomplèteslocales, preuvesGitHubcompactes soussix-hour/recovery-powercut (manifestes/extraitsréels, nepermettentpasde recalculerleSHAdatasetentier). AucuneimportationPSD/vitesse/RMSnormative exécutée ; suiteanalytique sousmissionR5àdéfinir, montageprototype/orientation/horlogeàborner. Ne pas adopter implicitementv4 ni modifierFIR/gain/plage. Le fichier réellementretrouvéestE6N5 ; E9N5dansl'ajoutR5ae144étaituneattente sousréserve.

Les procédures ethelpers de lancement antérieurs sont des archives de cettecapture déjàexécutée. Ne pas les rejouer àl'ouverture decesnotes. Repartir des copies etrésultatsprésents ; nouvellecapture uniquement sousinstruction/contrat explicite.

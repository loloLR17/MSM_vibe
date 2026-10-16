# R4-M — isolation de la trace STARTED sur banc fermé

Cette procédure de qualification est autorisée uniquement par la mission
TR2-20261016-R4M-LIVE-FRAM-RECONCILIATION. Elle ne définit **aucune récupération
automatique du produit** et ne modifie pas R0. Ancien START reste indécidable ;
aucun 28/29/COMPLETED n'est fabriqué pour lui.

## Préconditions et analyse

TR2-001 UID004A003D/38425018/20333157, ST-LINK003500463235510B37333439.
Double sauvegarde fraîche Flash2097152/FRAM262144, fichiers relus/comparés :
Flashc5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,
FRAMc814e9cc32eff06016f28ef964233354e35ba458653a16434e4416b296271561.
Archive FRAM **intégrale, sans perte**,1273octets zlib, disponible dans
`tests/fixtures/r4_started_snapshot.zlib`, décompression SHA vérifiée. Sauvegardes
binaires et Flash restent aussi dans `/home/lolo/dev/msm/reports/TR2-r4m/hardware/`.
Les sauvegardes initiales R4 et sa mission/rapport sont préservés intégralement.

Racine courante LIVE génération4, payload CRC vérifié. Journal slot0 : copieA
RESERVED génération1/CRC05cd4bb6 ; copieB STARTED génération2/CRC73a97135.
Clé(epoch1,txid1),code13,run1,config2/gen2. TR2E contient deux **INTENT** authentiques,
générations1/2, jamais un reçu START_OBSERVED. Il n'y a pas de preuve permettant
une terminalisation rétroactive. `inspect_live_fram.py` décode copies/CRC/racines ;
les vrais stores R1/R2 confirment recovery9 dans le test de maintenance.

Capteur relu sans écriture : WHO7B,CTRL1=00,FIFO_CTRL4=80(bypass),CTRL10=00.
Le debug minimal nécessaire aux doubles sauvegardes est l'accès de phaseA ;
aucun flash, reset ou diagnostic modifiant la cible avant leur vérification.
Avant publication maintenance : stop/readbacks répétés par l'owner réel.

## Transition contrôlée et anti-rejeu

1. Firmware isolé `TR2_STM32_R4_BENCH=ON` **et**
   `TR2_STM32_R4M_MAINTENANCE=ON`, artefact NON-PRODUCTION. Aucune entrée RTU/SD.
   **Chaque boot commence CLOSED et WAIT** ; seules commandes RAM4/5 reconnues.
   Le trigger4 exige le SHA intégral original et un token de sauvegarde posé par
   le lanceur après vérification des fichiers. UID/horloges/allocation vérifiés.
2. Capturer le verrou R4-M seulement si l'unique transaction est exactement ce
   STARTED, epoch1, avec INTENT authentique correspondant. Sans cet INTENT, pas
   de borne run_id prouvée : refus avant écriture, aucune valeur inventée.
3. Nouveau contexte de qualification : vider **logiquement** les28672octets de
   journal dans une candidate, par l'API du média existant, puis commit atomique
   payload→readback/header→root. Seule l'allocation LIVE peut être écrite ;
   ancien bank publié intact jusqu'au nouveau root. Epoch, configurations,
   identité/provenance et les128octets TR2E sont conservés byte-à-byte.
   L'ancien STARTED reste archivé, pas résolu. Une seule tentative d'isolation.
4. État de pont **RETIRING** : frontend fermé à toute commande ordinaire ; seule
   l'opération interne RENEW(epoch1,id65535) est autorisée. Le vrai moteur R2 écrit
   ses propres RESERVED/STARTED/transitionN+1/COMPLETED, avec ses barrières durables.
   Aucun epoch sauté ou record de transition construit à la main.
5. Ouvrir le frontend de qualification seulement après preuve du vrai RENEW
   terminé4/0, current2/previous1/flags0 et TR2E intégral inchangé. L'ancien tuple
   est alors refusé24 par R2, sans écriture ni acquisition. Prochain run réservé2,
   grâce au véritable INTENT1 conservé. Aucun run_id/epoch consommé n'est réutilisé.
6. START2/1/run2, RENEW2→3, service60s, STOP3/2, APPLY3/3/config3, START3/4/run3.
   Après ce dernier COMPLETED seulement : erreur de service8 injectée par logiciel,
   FIFO/arrêt réels, vérification du STOP indépendant et d'absence de nouvelles
   écritures. Ce n'est pas une panne électrique SPI ni un WCET de bus défaillant.
7. Déclencheur5 **readonly** après reset : SHA final prédit par modèle des vrais
   stores, vérification réelle de récupération, aucun START automatique ni write.
   L'oracle de métadonnées ne prouve pas les effets capteur : ceux-ci exigent les
   observations réelles du port STM32 pendant le RUN4.

**Limite essentielle :** le pont, bien que format-valide, n'est pas une politique
sûre de boot produit sans ce frontend. Contrôle négatif : appeler directement
l'API normale dans le pont réadmettrait l'ancienne clé. Ce chemin n'existe pas
dans le firmware de maintenance ; pas d'UART, pas d'intake normal, unique appel
interne whiteliste RENEW. Après reset, BSS remet CLOSED ; le SHA original ne
correspond plus au pont, donc trigger4 refuse toute nouvelle écriture. Aucun
firmware LIVE générique ne doit être chargé sur un pont interrompu. Les images
V1/M016 autorisées ignorent LIVE ; toute reprise exige inspection et nouveau plan,
jamais répétition d'effacement jusqu'à PASS. Cette restriction appartient à la
procédure de banc explicitement isolée, pas à une extension silencieuse de R0.

## Validation, traces et échecs

Test nouveau sur snapshot authentique, vrais stores/codecs/moteur :142budgets de
coupure du commit d'isolation (payload stride4096, chaque octet header/root),
quatre phases de coupure RENEW, anti-rejeu, conservation INTENT, prochaine run2,
refus sans INTENT et refus d'une seconde isolation. Frontend CLOSED après chaque
boot ; aucun dispatch capteur dans ces reprises. Contrôle négatif conservé.
Métadonnées de phaseC simulées indépendamment du temps physique pour comparaison
intégrale future ; simulation ne fabrique aucun reçu sur cible.

Sur cible : journal de chaque plage d'adresses écrite (quanta contigus agrégés,
phase, taille, nombre de commandes, résultat) ; refus avant dépassement du log.
CS relâché observé via ODR après chaque write. Comparaison intégrale historique/tail,
latences des opérations et STOP, maxima work/service/FIFO/gaps. Aucun seuil R4
relâché : quantum32,work1000us,service2500us,gap5ms,premier-mot20ms expérimental.
Aucun provider temps capteur calibré, donc aucun B3/protocole2 annoncé.
Un nouveau STARTED indéterminé impose arrêt et conservation, pas réinitialisation.

## Restauration et préservation des identités consommées

Après essais : sauvegarder intégralement FRAM nouvelle, comparer zones protégées,
puis restaurer **Flash initiale seulement**, pages modifiées depuis backup et
lecture complète2Mio. Ne pas rembobiner FRAM : epochs/runs nouveaux restent consommés.
L'ancienne trace est conservée hors cible ; son retour n'est pas un rollback produit.

Procédure FRAM de secours préparée, **non exécutée par défaut** : banc hors du
chemin de commandes, firmware M016 ignorant LIVE, capteur arrêté prouvé ; choisir
la sauvegarde intégrale vérifiée la plus récente et consulter le registre de
contexts/identités déjà consommées. Comparer historique/tail avant toute action.
Écrire par quanta bornés seulement[122880,245760), banques/gaps d'abord, vérifier,
racines en dernier, puis comparer les262144octets. Journaliser chaque adresse et
conserver l'état courant avant remplacement. Pendant toute interruption, aucun
firmware LIVE générique ni émission de commande normale. Une archive plus ancienne
peut être restaurée pour forensic uniquement, jamais pour réutiliser un epoch/run
consommé ultérieurement : maintenir la quarantaine et faire arbitrer la réconciliation
des bornes avant activation. Si la borne maximale est inconnue, rester fermé.
Aucun mass erase, changement OptionBytes/RDP, écriture historique/tail ou SD.


## Reprise explicite du pont après le premier essai R4-M

Le premier essai a publié le journal isolé, puis s'est arrêté avant RENEW sur
1075us >1000us pendant recovery. Aucun nouveau START ni epoch/run consommé.
Snapshot intégral réel `c868950fe3c60f9a3b23a9ce8d565029ee57d4f06b08cbf6062de9238b2af950`,
protégés identiques. La relecture seule a réussi ensuite (824us maximum) : cause
précise non attribuée ; validations CRC/décodage successives ont des checkpoints
supplémentaires, sans supprimer aucun contrôle ni augmenter les budgets.

Un **nouveau plan explicite de reprise**, pas un retry de reprovisionnement,
est borné à ce seul SHA intégral et au trigger6 distinct. Chaque boot reste
CLOSED/WAIT. Le token exige sauvegardes vérifiées du pont. `r4m_capture_bridge`
exige journal entièrement absent, epoch1/gen1/flags0, aucun actif/last/indéterminé,
INTENT run1/config2/gen2 authentique, et copie128 byte-identique à l'archive initiale.
Il arme RETIRING ; **aucune isolation/écriture de zéro n'est appelée**. Seul le
vrai RENEW1->2 peut ensuite ouvrir la campagne. L'ancienne clé est reconstruite
comme requête de contrôle négatif, jamais comme reçu. Une nouvelle indétermination
impose l'arrêt, sans nouveau plan de nettoyage. Ce chemin n'est jamais automatique
ni disponible dans le firmware produit ; il n'est valable que pour le banc fermé.


## Résultat physique et fermeture du plan

Le trigger6 a réellement terminé RENEW1->2/4/0 et rejeté l'ancienne clé24/0write.
Le START2/1/run2 a ensuite échoué sur1241us>1000us, avec nouveau STARTED indéterminé9.
**Arrêt contractuel : aucun second nettoyage ni nouvelle campagne autorisée par ce plan.**
Snapshot final intégral f2bd7a99d3d0e9f17292310f2cec51fe3a3f521ba3a4d2f07da5b6302258a0c1,
rootgen11 et INTENT run2,0XYZ. Les SHA autorisés par les triggers refusent ce snapshot.
Mode5/reboot final,60s/FIFO,STOP de commande,APPLY/faute injectée non exécutés.
Flash initiale restaurée exacte2Mio ; FRAM finale conservée sans rembobinage.
La reprise exige un nouveau contrat/plan, pas la réexécution des scripts de ce plan.

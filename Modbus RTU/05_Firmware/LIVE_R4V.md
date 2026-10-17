# R4-V — diagnostic et correction de latence START/STOP

Mission TR2-20261017-R4V-RTU-START-STOP-LATENCY. Aucun seuil d'arrêt industriel nouveau décidé. R0/B5/R1/R2/barrières/formats/anti-rejeu inchangés.

## Préparation historique — avant les essais R4-V

Captures R4U relues intégralement :1632ADU, START63,100s/STOP63,249s terminaux ; ACKSTOP→premierHELD55,757s. MCU instant d'effet et temps cumulés service/I/O non observés alors ; aucune attribution de63s à la FRAM seule. Le code fait des callbacks coopératifs avant/après chaque quantum32octets et dans CRC/copie/hash/recovery. Un drain implique SPI/status/config/horloge même si peu de FIFO est arrivée. Hypothèse : sur-service coûteux amplifie le temps des barrières ; à mesurer, pas une cause déjà prouvée.

## Composition et instrumentation

R4V explicite requiert recetteR4U/duplex4MHz, artéfact NON-PRODUCTION distinct, bootWAIT.96événements/32spans RAM bornés ; aucune impression ou GDB pendant transfert. MCU now_us(DWT suivi), événements capture observée après traitementRTU, callbackSTART/STOPdébut/fin, observation capteur, premier B3HELDpublié ; spans avant/après chaque pasR2 avec journal/root, service cumulé et I/O physique séparés. Dernier XYZ consommé/provider distinct du dernier XYZ produit (FIFO non lue lors du STOP). ACK/HELD/terminal maître mesurés séparément par captures COM4. B1 reste non supporté : aucune fausse publication de sa valeur. La capture vue aprèspoll est bornée par le service, pas datée comme l'arrivée électrique.

Profils comparables :
- baseline0 : mêmes callbacksFIFO à chaque checkpoint queR4U, instrumentation passive ; exige FRAM95da…9af41/journal46/root59/run7terminal, nouvelles clés5/5START/run8 et5/6STOP.
- candidat250 : même code et instrumentation, espacement seulement du drain coûteuxFIFO ; exige exactement snapshot aprèsbaseline prédit horslignefce8e1f9330989e02f7139d10cb066383bca1c58447a00a9852a8ca6c4b4a682/journal54/root67/run8terminal. Clés5/7START/run9 ; clé5/8STOP refusée une fois par budgetbench négatif, sans commit/effet ; puis5/9STOPneuf. Aucun budget positif fabriqué.

Le pacing reste désactivé par défaut. API seulement arrêtée, plafond un quart du work_limit existant, soit250µs. UART8événements et deadline restent traités à chaque checkpoint, publication10ms inchangée ; aucune barrière déplacée/supprimée. Borne conservative nominale avant qualification :250+1000+2500=3750µs<gap5000 ; les mesures réelles restent requises. Le service de sécurité ne dépend pas d'un reçu FRAM réussi. Toute erreur du foreground utilise le fault R2 existant (stop/invalidité) puis publie le snapshotB3invalide, sans reçu terminal fictif.

## Tests et résultats préalables

145tests complets PASS, incluant les144anciens/1561coupuresR2 ; nouveau pacing0/250, frontière horloge avec coût modèle4µs, refus du changement en acquisition, retry/collision, budgetSTOPabsent sans effet tardif, erreurFRAM, faute de service et B3invalid. Le contre-test faute de service échouait avant la correction de retraitB3, puis passe ; deux erreurs initiales d'oracle (lecturehorloge modèle4µs et retourprojectionB3enfaute) sont documentées, pas des contrôles produit affaiblis.4suitesO2/ASan/UBSan PASS6,50s, ARMcanonique/dédiés PASS. FirmwareV1/protocole1 conservé.

## Préflight et exécution conditionnelle

TR2-001 SN003500463235510B37333439/UID004A003D3842501820333157, M016WAIT/Flash2Mio c5bc…f5b26 et FRAM95da entière vérifiées fraîchement10:18:02UTC, capteurWHO7B/ODRoff/FIFObypass/TSoff. COM4FTDI0403:6001/BG04KSSW, COM3STLINK ; même raccordement confirméR4U, maître exclusifC# natif, aucunHP/SD.

Mesurebaseline nécessaire pour les jalons absents R4U, pas répétition du simplePASSfonctionnel. Après succès intégral et FRAM exacte au modèle, mesurer candidat ; sinon STOPsûr/archives, pas de nouvelle identité arbitraire. Ne jamais rejouer une clé admise ni retireR4T. Les modèles de métadonnées ne prouvent aucun effet physique à eux seuls.

Après probes, restaurer M016 par pages normales couvrant la plus grande image (calculer19pages selon taille réelle), relire2Mio et FRAM entière finale, vérifier capteur arrêté/detach/COM4fermé. Pas de rollbackFRAM/OB/RDP/masserase. RecoveredR2boot ne doit pas autoSTART ; gatesSHAexactes avant stores.

Aucune qualification physique R4V anticipée : objectif quantifié après diagnostic, pas de seuil industriel inventé. Si un arrêt rapide nécessite déplacer une barrière normative, décision humaine séparée obligatoire. R5/SQLite/Web et R6 restent hors qualification.

## Baseline physique instrumentée

Codef8bf7ca/profile0 : START5/5/run8 etSTOP5/6COMPLETED4/0,1655réponsesCRCexactes. FRAMentière fce8…a682 exacte au modèle/journal54/root67/epoch5/run8. Work815/service1624/gap1852µs,0erreurFIFO. DepuiscaptureMCU : STARTeffet7,678s/terminal64,281s ; STOPeffet56,628s/terminal64,319s.82,13–82,14% du temps des spans appartient au service coopératif,6,980s/commande à I/O physique. Sur-service dominant prouvé, pas «FRAM63s». CaptureaprèstraitementRTU et tempsDWTnominal distincts des ACKmaîtreUTC/Stopwatch ; aucune synchronisationSI prétendue.

Candidat : délai250µs après FIN du callbackFIFO (pas après début, qui pourrait rendre un drain lourd immédiatement à nouveau dû). UART/publication/deadlines ne sont pas retardés. TestcacheSTOP reproduit FRESH retenu avant10ms : publication immédiate sur transitionrunning/error corrigée sans changer les barrières. À ce checkpoint historique : tests en cours, candidat non flashé. Voir le résultat final ci-dessous.

## Candidat prêt pour physique

145tests complets et cross-build finalPASS ;4suitesASan/UBSanPASS8,69s. Modèle : pacing0service260403/drain260403, work484/service498/gap613µs ; pacing250service260405/drain80494, work484/service767/gap1072µs. Ces temps sont simulés, pas des mesuresSTM32. Le contre-test STOPavant10ms et les fautesFRAM/service passent, aucunfauxFRESH ni reçu fabriqué. Sourcefinale qualifiée logiciel ; seul le candidat250àfin_de_drain sera utilisé, première variante250début_de_drain construite maisjamaisflashée conservée séparément. Aucune autre phase baseline n'est nécessaire.

## Bilan physique final — 17 octobre 2026 UTC

Référence instrumentée réellement exécutée : `f8bf7ca80a7dad385dd84d00ffecea7e3dea125b`, profil0. Candidat réellement exécuté : `8db2f673ac46c3e4d2bb64f0bbd39ca16bd39a4e`, profil250 après fin du drain. Deux acquisitions distinctes et nécessaires à la comparaison ; aucune répétition après l'interruption de session. Première variante250 calculée depuis le début du drain jamais flashée. Source et empreintes détaillées dans les preuves `images.json` et `images-final.json`, BIN/ELF exécutés conservés dans BUNDLE.

### Chronologie causale mesurée

Temps en secondes depuis la capture observée MCU de chaque nouvelle commande ; capture après traitement RTU et mise en file de l'ACK, **pas** arrivée électrique du premier octet. DWT converti à160MHz nominal : aucune calibration SI ni synchronisation avec l'horloge maître. Les événements complets conservent aussi les temps absolus, générations et phases.

| Jalon MCU depuis capture (s) | Référence START5/5 | Candidat START5/7 | Référence STOP5/6 | Candidat STOP5/9 |
|---|---:|---:|---:|---:|
| Admission durable | 3.838259 | 3.907510 | 28.302225 | 16.534928 |
| STARTED durable | 7.677156 | 7.815903 | 56.625249 | 33.028025 |
| Début callback matériel | 7.677282 | 7.816027 | 56.627876 | 33.028146 |
| Fin callback matériel | 7.678087 | 7.816831 | 56.628081 | 33.028351 |
| Observation capteur achevée | 7.678263 | 7.817398 | 56.628195 | 33.028870 |
| Reçu durable | 35.971379 | 24.319500 | 60.473575 | 36.943933 |
| Terminal durable | 64.280557 | 40.809834 | 64.318544 | 40.858764 |

| Mesure distincte | Référence0 | Candidat250 |
|---|---:|---:|
| Callback START / STOP (µs, événements début/fin) |805 /205|804 /205|
| Fin callback STOP → publication HELD MCU (µs) |1242|429|
| Capture MCU → publication HELD (s) |56,629323|33,028780|
| ACK STOP → premier HELD, maître (s) |56,354535|32,866055|
| START terminal, maître (s) |64,037795|40,644804|
| STOP terminal, maître (s) |63,991567|40,677670|
| Work / service / gap FIFO max (µs) |815 /1624 /1852|814 /2063 /2274|
| FIFO niveau max / erreurs |55 /0|68 /0|
| Réponses COM4 CRC exactes / V-F / HELD |1655 /740 /50|1075 /447 /51|
| XYZ / timestamps |3084000 /385500|1833115 /229140|

N4096 :35 séquences au dernier snapshot candidat, oracle indépendant sur les4096 XYZ bruts : RMS global/X/Y/Z958/955/75/13mg, peaks998/994/110/50mg, exacts aux sorties MCU. Les échantillons sont réels, leur contenu vibratoire n'est pas une calibration métrologique. Dernier XYZ **consommé/daté** candidat84767245µs ; arrêt callback84768764µs, écart1519µs. Aucun timestamp de dernier XYZ effectivement produit n'est mesuré. HELD conserve la dernière fenêtre sans FRESH ; B1 non supporté, aucun jalon B1 inventé.

### Cause prouvée et latence résiduelle

Les quatre barrières de chaque commande sont conservées. À l'arrêt, chaque barrière coûte environ3,84s référence et3,91s candidat. Avec acquisition, elle coûte28,3s référence contre16,5s candidat. Le service coopératif répété occupe82,13–82,14% des spans R2 de référence et71,38–71,41% candidat ; I/O physique reste6,980s par commande. Pour STOP candidat :40,859791s dans les spans, dont29,178062s service,6,979826s I/O,4,701903s résidu CPU/instrumentation. Le résidu est calculé par soustraction, pas un profilage exclusif des fonctions CRC. Les deux barrières pré-effet expliquent l'attente avant l'arrêt ; les deux post-effet expliquent le délai restant jusqu'au terminal. Le callback STOP n'est donc pas responsable des33s d'attente.

Cause dominante reproduite : services FIFO/SPI coûteux déclenchés à chaque micro-quantum CRC/copie/FRAM, même lorsque peu de données nouvelles sont arrivées. Espacer uniquement les drains coûteux après leur fin réduit STOP terminal MCU de36,47% et ACK→HELD maître de41,68%, sans déplacer les barrières, baisser les budgets ni modifier R2. La latence résiduelle de33s jusqu'à l'effet et41s jusqu'au terminal reste importante ; ce résultat accepte uniquement la portée du diagnostic comparatif, **pas** un arrêt industriel rapide ni un GO R5/R6.

Le plafond250µs conserve un gap physique2274<5000µs ; service2063<2500µs etwork814<1000µs. Ce sont des maxima observés, pas un WCET prouvé. Le délai configuré est un minimum entre fin d'un drain et admissibilité du suivant ; il ne constitue pas une période fixe. UART/deadlines continuent à chaque checkpoint.

### Refus, sûreté et causalité persistante

START5/7/run9 etSTOP5/9 sont terminaux4/0. Retry exact5/7 sans nouvel effet, collision STOP5/7 refus27. STOP5/8 refusé5/7 en2,591ms de span MCU (service cumulé2,515ms, à distinguer du maximum d'un callback2063µs) : I/O0, journal58→58, root71→71, acquisition toujours active. Le refus est un stimulus négatif de banc, aucune permission positive fictive ; il ne prétend pas arrêter la carte. STOP5/9 neuf passe ensuite les vrais budgets.

Les fautes FRAM/service et l'arrêt sûr indépendant d'un reçu sont testés sur modèle, avec contre-tests avant/après correction. Aucun défaut électrique FRAM ni coupure physique n'a été injecté dans ce banc : effets et WCET de ces fautes non qualifiés sur cible. Gardes conservées, pas de reçu fabriqué. Deux publications corrigées : retrait immédiat FRESH après changement running/error et publication invalide après fault existant. Pacing désactivé par défaut en production.

### Persistance et fermeture

Chaque phase écrit419408octets LIVE/8commits,13112 appels physiques d'écriture ; les lectures/écritures réelles restent comparables, hors instrumentation et préflight. FRAM complète exacte aux modèles construits avec le vrai store/R2 :95da initiale → fce8 baseline →70e273972a83e61283a6e572db3187af002fd97ba8de63f8c709d3e30af73587 finale. Journal46→54→62, root59→67→75, epoch5 inchangé, run7→8→9, config3/gen3 ; effet39phase3/STOP5/9. Anciennes clés et runs jamais réutilisés, aucune retraite/RENEW/reprogrammationFRAM. Historique[0,122880) et tail[245760,262144) intégralement identiques.

M016 restauré à10:52:16UTC par19pages normales/155648octets de la sauvegarde initiale vérifiée ; lecture complète2097152octets exacte SHA c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26. FRAM262144octets relue après restauration exactement70e273…73587 ; WHO7B/ODRoff/FIFObypass/TSoff lus ensuite, CommissioningReadOnly WAIT, GDB détaché/COM4 fermé. Aucun SD/HP/câblage/OptionBytes/RDP/mass erase/rollback. Aucun nouveau test réussi rejoué à la reprise.

### Suite proposée au contrôle indépendant

Une latence de sécurité acceptable doit être décidée avec la sémantique de STOP durable et l'analyse de risque, sans seuil inventé ici. Optimisations bornées CPU/CRC/copie et coût de service peuvent faire l'objet d'un nouveau contrat et d'oracles identiques. Réduire le coût des enveloppes/journaux persistants par changement de représentation, déplacer une barrière ou ajouter une voie d'arrêt d'urgence indépendante exigeraient une décision d'architecture et, selon le choix, une évolution normative explicite. Aucun de ces changements n'est appliqué.

R5 doit encore couvrir profil/version/capabilities, B1/B2/B7, corrélation run/config sur fil, décodage/SQLite/Web ; B6 demeure UNAVAILABLE et capacitéLIVE16 absente. R6/Nsupérieurs/endurance, fautes physiques, WCET IRQ/HAL, pile et dérive SI/thermique restent non démontrés. Le firmware de banc n'est pas un produit LIVE complet. Code final145tests/ARM PASS et4suitesASan/UBSan PASS8,69s déjà acquis, non relancés après documentation/restauration. V1 protocole1 conservé et BIN canonique85b1…c0f5 inchangé, non flashé.

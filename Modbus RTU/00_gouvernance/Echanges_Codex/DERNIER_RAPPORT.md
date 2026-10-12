---
mission_id: TR2-20261012-M015-L-ST-BENCH-PHYSICAL
status: BLOCKED
base_ref: main
base_sha: fccb40cf799c753b9897e74ae6cc6e737a07383c
initial_head: b64e8a6d4db17685223f7d761c52fa9d372b61cc
result_sha: e222dc8ddb7cfd0829c655533ac8e646220721f2
created_at_utc: 2026-10-12T20:31:59.566616+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-L — WHO essai n° 2 conforme, polling validé, FIFO long refusé

**Le blocage WHO est levé après la correction du câblage annoncée par
l'utilisateur : essai n° 2 unique = `7B`. Polling ST : 64 XYZ réels, non figés,
sans saturation, contrôles conformes.** Le FIFO long reste **BLOCKED** : première
lecture896octets STATUS10 refusée avec HAL_TIMEOUT/SPI_OVR. La seconde carte
est restaurée intégralement et laissée en exécution ; TR2 principal jamais accédé.

## Identité, autorisation et séparation des essais

Reprise utilisateur explicite après correctionSPI, autorisant **WHO n°2**, puis
polling/FIFO si conforme. Cette instruction lève l'interdiction de rejouer le
WHO consommé dans le premier rapport. ContratGit M015-L READY inchangé ; HEAD
réel `b64e8a6d4db17685223f7d761c52fa9d372b61cc` égal au distant, worktreeL propre. AGENTS/protocole/journal,
rapport et qualification L relus ; règles et sources inchangées.

**Essai n°1** : `00`, preuves du dossierparent conservées sans changement,
[rapport archivé exact](archives/TR2-20261012-M015-L-ST-BENCH-PHYSICAL/RAPPORT.md)
depuisb64e8a6. **Essai n°2 après correction du câblage** : preuves dans
[essai02](preuves/TR2-20261012-M015-L/essai02/README.md), originaux séparés
**/home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z**. Aucun mélange des payloads/chronologies ; correctionphysique rapportée
par l'utilisateur, détail du fil corrigé non communiqué.

Sonde unique identifiée explicitement **0032001F3235510C37333439/V3J15M7**,
seconde NUCLEO-U575ZI-Q. UID relu **001B00463842501820333157**, MCU482/RevU/3,28V
CLI ; identité principale SN003500463235510B37333439/UID004A003D3842501820333157
refusée. GND/VDD/VDDIO3,3V et conformité du banc précédemment confirmés,
correctionSPI annoncée ; pas nouvelle mesure physique indépendante revendiquée.

**Flash2MiB fraîche relue deuxfois identique** avant programmation et identique
à la restauration de l'essai1. SHA
**fcea004eb0c91af44f8e36ca61206b5f6bf25fd4c5db4dfdab42d1a19dcea9d9**. RAM conservée.
Les quatre artefacts K rehashés exacts, firmware-source **e222dc8** inchangé ;
41scénariosC/5testsPython/4buildsARM acquis, **aucun test/build rejoué**.
Helpers de capture corrigés avant reprise pour lire GPIOC par adresseCMSIS,
sans changer le firmware ou les contrôles. Permissioncommit/pushmain/flash/debug
exercée dans le contrat et la chaîne seconde cible qualifiée.

## Résultats séquentiels réellement observés

| Étape distincte | Résultat | Preuves / limites |
|---|---|---|
| A — WHO n°2 après correction | **Conforme :7B/result0/cleanup0** |1traceHAL_OK, zérowritecapteur, triggerunique |
| B — Polling ST | **Conforme :64XYZ/result0/cleanup0** |64triplets distincts, 0saturation, 309transfertsvalides |
| C — FIFO ST_STRICT | **Anomalie getter reproduite :ST_LEVEL8** |API0/statutbrutsuivant257/watermarkactif/0drain,840tracesvalides |
| C2 — FIFO STATUS10, variante déclarée | **Refus transport :IO1** |premierburst demandé128entrées/896octets, HAL_TIMEOUT3/error104,0entréeacceptée/cleanup0 |

Chaqueimage a étéflashée NORMAL/SNexplicite/08000000/verify/readbackexact,
resetGDB aprèsdernierCLI puisGDBseul ; bootready1/UIDexact/core16MHz/faults0,
untrigger par étape, detach pendant exécution. Aucune répétition après le refusC2.
Les WHO internes des séquences ST B/C/C2 sont leurs contrôles d'initialisation,
pas des relances de l'essai WHO autonome. Les réglages constructeur sont
inchangés : pollingFS2g/BDU/ODR/LP_ODR_DIV100 ; FIFOFS8g/watermark256/BDR0A/
STREAM6/DEC8/timestamp. Pas fallback de mode ou correctionheuristique.

Polling brut : X1462..1566, Y−837..−732, Z15974..16138 LSB ;64tripletsdistincts,
aucunsaturation. CSV64 et384octetsXYZ reconstruisent exactement les valeursRAM
et payloads des64lectures0x28. SHA
**1ff9da3d6d80a5afe6623506b07c25d51b5b67aec119790285ee6598327dbfd3**. TimestampsMCU sont débutsd'appels, pasconversion.
Pas excitationexterne/immobilité/calibration présumée, pas cadencecontinue V1.

FIFOstrict : getterST masque03F conservé ; dernierstatutgetter correspond auniveau
256 maisdonne0, lecturebrutesuivante257/flags81. Lesdeuxtransactions ne sontpas
simultanées. Contrôles07..0A=00/01/0A/86 etWHO7B/CTRL1AC/CTRL344 conformes ;
getterbatchNOT_BATCHED conservé comme télémétrie, gardeBDRbrut0A satisfait.
Les anomaliesdegetterK sontdonc réellementobservées, nonmasquées. AucunpayloadFIFO
lu enC ; STATUS10 étaitla variante distincte prévue et autorisée par la reprise.

## Premier refus FIFO STATUS10 : preuve et analyse hors ligne

Trace **834** (index0-based) : reg78/896octets demandés, commandeF8 reconstructible,
**HAL_TIMEOUT3, error0x104 = HAL_SPI_ERROR_OVR0x04 + HAL_SPI_ERROR_TIMEOUT0x100**.
Tick4739→premiercleanup4759, cohérent avecborne20ms ; paschronométragewire.
Dernierstatutavantlecture : niveau256/flags81, aucunFULL/OVRcapteur dans les
statuts observés avantdrain. **Pas de statutaprèsburstéchoué** : nepasconclure
à absence globale deperte/overflowcapteur.

**OVR concerne le SPI STM32**, pas le flagFIFO IIS3DWB. HALSPI vCubeU5v1.9.0
litSPI_SR à `SPI_CloseTransfer`, consigneOVR, puis remet **Tx/RxXferCountà0**
même surtimeout. Le validmask0/refusHAL estconservé : compteursfinaux0 neprouvent
pas896octets reçus. SourceHAL et constantes exactes dans `HAL-error-excerpts.txt`.

Le buffer896 étaitpréalablementzéro, partiellementmodifié ; nombresréelsd'octets
reçus **non enregistrés avantfermetureHAL**. Hexcomplet publié commebufferrejeté,
byte-identique entrepayloadtrace etslotsRAM ; nepasleprésenter comme128mots
reçus ouvalides. AucunCSV128XYZinventé, aucune fenêtre4096. Les zérosrestants
sontnonqualifiés, pasdesmesures ; tag/timestampapparence nequalifiepascebloc.
Le code n'atteintpas leparseurtags/compteurwindow, count/xyz/timestampsacceptés0.

Capacitésnonépuisées :841traces/1024,2548octetslogs/65536 ; échec au premierburst,
pas gardeRAM. Cleanup6transactionsHAL_OK : XLoff/bypass/timestampoff commandés.
Aucun nouveau readbackconfiguration validé après ce burst ; nepasrecycler la
snapshot préburst comme preuve postburst. CPU16MHz observé, SPI2MHzcalculé,
Debug `-g` sans `-O` dansninja. **BudgetserviceRX insuffisant est une hypothèse**,
pas cause détaillée démontrée ; OVR+timeoutHAL estfait. Aucunpatchet essairejoué.

## Restauration et conservation

Avantrestauration, Flashentière lue, hors32KiB byte-identique au backup. Seuls
secteurs0..3 restaurésdepuisbackupfrais32KiB ; verifyCLI puisreadback **2MiB
entièrement byte-identique** au backupfrais etrestaurationessai1, SHAfcea004e… .
DernierCLI suivi deresetGDB puis lecture sansreset : UIDexact, PC08000c80,
VTOR08000000/CFSRHFSR0, detach/serveursexit0. Firmwareinitialnonidentifié
fonctionnellement : restaurationdesbytes, pasqualificationapplicative.

Quatreflashsréférence et unflashrestaurationstandards, toussursecondecarte;
aucunmass erase/OptionBytes/RDP/HOTPLUG/média/COM7/RS485. **M014-H/FRAM/SD du
TR2 principal préservés, aucuneconnexion à ce dernier**, aucune repriseHP requise.

## Comparaison I/J, limite et prochaine vérification précise

L'échecWHO n°1 est levé après correctiondéclarée, avec mêmeSN/UID/imageWHO.
Cela concorde avec un rôle du câblage, sans établir quelleconnexion niune panne
capteur. Le secondbancdémontreWHO etpollingSTmode0 ; il nequalifiepasFIFOlong.
I256figé restevalidé/Jpostburstconfigurationrefusée restentacquis surautrebanc,
mode3/CPU160MHz/SPI2,5MHz etHALduplex. Leurcause n'estpasidentifiée par cette
comparaison multifacteur ; aucuneattribution àbreakout/SPI3/HAL précise.

**Prochainevérification logicielle ciblée** : instrumenterSPI_SR et RxXferCount/
pRxBuffPtr **avant** SPI_CloseTransfer, mesurerbudgetserviceRX pour4µs/octet
à2MHz, examiner cheminHALReceive896, packing/seuilRX, CPU/optimisation.
ConserverFS/ODR/watermark/batchingST, limites etrefus ; nepasallonger simplement
timeout pourmasquerOVR. Préparer unprofil de plateforme justifié etvalidé hors
cible avantnouveaucontrat/campagnebornée. Aucune nouvelleinterventionphysique
n'est actuellement identifiée comme indispensable ; vérificationlogicielle
possible, pas clôture pour simple confirmation manquante de câblage.

Journal/roadmapactualisés : WHOetpollingconformes, getterSTreproduit, FIFOlong
BLOCKED surOVRSTM32, restaurationsecondecible exacte, acquisI/J/M014 inchangés.
Preuvespubliques essai02 :108fichiers plusmanifest, originauxdansdossierci-dessus,
logsUTF8/LF normalisés ethashespublé/original, scriptsanalyseoffline.
Résultats publiés au commit **0eda53843e64becf8dfa5ae4ec02dfb23a04c77d**, `M015-L: qualify corrected-bank WHO and polling, record SPI overrun on FIFO`.
Push normal main réussi, fetch puis **115 livrables relus intégralement et
byte-identiques** ; premier dossier de preuves intact, archivesK et rapportL
n°1 exacts, mission inchangée, code/projetST inchangé, quatorze autres worktrees
et deux stashes préservés. Preuve `essai02/publication-readback.json`. WorktreeL
propre après ce push ; normalisation documentaire des espaces terminaux d'une
extraction HAL ensuite, original et hashes conservés sans modification des faits.
Le présent accusé sera publié et relu normalement ; SHA final et état Git dans
la réponse. **BLOCKED qualifie le FIFO long, pas WHO/polling désormais validés
dans cette portée.** Contrôle indépendant ChatGPT attendu.

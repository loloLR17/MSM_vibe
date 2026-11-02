# Comparaison physique sans SD — résultats et limites

Autorisation humaine explicite du02/11/2026 : voir AUTORISATION_ET_PLAN.md.
Source comparateur **6be825d4681a17224000ae1f2b088a315ba328c0** ;
BIN50928B SHA7886feb2371837ceef7e126f26b4dedb2d484538a6142b8df2ccd662cc0dfbaf.
TR2-002 SN0032001F3235510C37333439 / UID001B0046/38425018/20333157.
FS16/lectureCTRL1_XL04auWAIT, SPI3 8MHz, duplex, FIR107Q15/dec10, limites
résidu100µs/budgets2500/1000/5000/1000µs identiques à D01 sur001.
Seules adaptations fonctionnelles : sélection UID002 et départ RAMdiagnostique
0x44335202auWAIT ;001 conserve son USERobligatoire. Pas qualification binaireidentique.

| Essai | Durée MCU µs | Natifs acceptés | Sorties calculées | Checks compteur | Résidu max µs | Résultat |
|---|---:|---:|---:|---:|---:|---|
| D01/001, acquis |16659203|442872|44288|167|228049169|ERROR3|
| D02/002 |60000645|1584496|158450|599|19|PASS borné|
| D03/002 |60000639|1584376|158438|599|19|PASS borné|

Deux essais au total sur002 ; un reset logiciel qualifié entre eux pour vérifier
la répétabilité après réinitialisation, sans reflash ni répétition d'anciens tests.
GDB armé auWAIT, départ RAM puis CPU libre ; breakpoint terminal seulement après
STOP/snapshot. CRC des deux snapshots validés, source exacte incluse.
FIFOerror/stop_error/flags/rails/clips/queue_overflows=0, STOPpropre.
Budgetmask0 ; maxima enµs dans D02-analysis.json/D03-analysis.json ; sd_step représente
un appelwriter vide, pas une écritureSD. Tous les contrôles restent stricts.
SD physiquement insérée selon utilisateur sur002 mais **commands=reads=writes=0,
initialized=false, write_enabled=false** avant et après les deux essais ; writerinactif.
Pas de fichier créé. sd_total_bytes est un volume calculé en RAM, pas des données
écrites ; seuls diagnostics/snapshots/digests sont conservés, pas tous les XYZ.

## Préservation et état final

Avant flash : identité001B…, deux dumpsFlash2097152B identiques, RAM786432B conservée.
Image initiale reconnueN8G53032B/codefbbdde5d328de47922cba688b6e406a327eb13f9,
SHA61afdfa92f84f7d9a194490a1cda49647e3be1b4d75d023bcff57015d89c75f0.
Au départ WAIT2/run0/error0, FIFO/writeroff et aucun accèsSD. Pas de capture en cours.
Après essais : programmation standard de cette image initiale puis readback **complet
2097152B strictement identique à la sauvegarde**, SHA
113f54b740a2ee2be789ac42649becb8fe58b8a47d33890c587faabf405386e5.
RetourN8G WAIT2/run0/error0, FIFO/writeroff, zérocommandeSD ; GDBdétaché.
Cette restauration ne requalifie pas N8G et ne lance aucune acquisition longue.
Aucune commande FRAM/COM4, masserase/OB/format ou changementWindows.
001 contrôlé enlectureseule : mêmeBIN D01 entier, ancien99/error3, FIFO/writeroff,
zérocommandeSD. Aucun flash/reset/START001 durant cette comparaison.

## Ce qui est démontré et suite

La chaîne002 exécute deux fois pendant60s ce diagnostic FS16/FIFO sans défaut,
alors que001 a reproduit sa lecture decompteur incohérente en16,66s sans SD.
La SD/sontrafic ne sont pas nécessaires au défaut001 ; échanger de carteSD n'est
pas la première action discriminante. Le défaut001 n'est pas reproduit sur002
pendant cette fenêtre ; le code commun n'est pas pour autant innocenté de toute
interaction dépendant des conditions. Les deuxmontages/capteurs/fils ne sont pas
identiques ni échangés dans cette tranche, et aucun signalélectrique n'est mesuré.
**Pas de preuve que le MCU001 est défectueux, ni de qualification endurance/stockage.**

Prochaine étape proposée : comparaison croisée des modulescapteurs entre les bancs,
hors tension et en conservant les câblages propres aux Nucleo, avec traçabilité du
module provenant du banc002 qui vient de passer. Un essai borné001 sansSD permettrait
de voir si le défaut suit le module ou reste sur sa chaîne. Cette manipulation est
à l'utilisateur, non effectuée ; pas de troisième essai002 ni nouvelle campagne ici.

Validationlogicielle proportionnée : buildsARM comparateur et profilnormal PASS,
refus CMake sanssensor-only et >60s observés ; absence des symboles SDinit/mount/
preflight/format dans l'image liée. Le script d'analyse des refus a d'abord manqué
un retour àlaligne CMake, corrigé par lecture des sorties réelles sans changer les
guardes. Pas de relance des suiteshôte anciennes inchangées.

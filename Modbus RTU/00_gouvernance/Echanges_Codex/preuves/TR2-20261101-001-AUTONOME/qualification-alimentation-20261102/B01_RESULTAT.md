# Palier1, première acquisition B01 sous ST-LINK seul — ÉCHEC

Source3f803d77393c6853ed76d796367df443a0eb9eb9, capteur8MHz/SD4MHz/±16g.
USER physique confirmé ; CN15 absent, JP6STLK/JP2retiré, GDB armé au WAIT.
Le breakpoint matériel terminal r3_finished est atteint après STOP/snapshot ;
aucun halt durant la capture, aucun reset ni reconnexion nécessaires au résultat.

Arrêt à17,088670sMCU, stage99/error3, garde de cohérence du compteur direct.
Brut FIFO1523255, lecture précédente1515331, lecture fautive19823754 dans
le bracket1049006171–1049006189µs. Les octets reçus8a7c2e01 décodent exactement
cette valeur ; le recalcul de la formule reproduit le rejet229404244µs/limite100.
Trace HAL complète : status/error0, RX/TXrestants0, cinq octets reçus y compris
commande, CSbas àpreclose ; cela ne prouve pas la fidélité électrique des bits.

Aucune erreur SD/FIFO ni saturation ; STOP134µs/sourcearrêtée, writerterminé,
nonincertain/CS_SDhaute/GDBdétaché. Masque budget0 ; maxima de polls complets :
service1446,79375µs/2500, traitement720,75625/1000,
intervalle1706/5000, SD456,39375/1000. Le passage fautif renvoie avant le calcul
final de budget ; ne pas inventer ses valeurs manquantes. Le refus observé est
celui d'horloge, distinct du dépassement A1H et du refus SD A1D.

Copie native PC2Mio/source=copie SHA944b50e38613f7d30fc345fb1f71605fbec62faa6cd6197b07ca9f192a7c48d7.
Préfixe576records/44928XYZ, +6recordsRAM vérifiés/+30XYZpartiels :45426sorties272556B,
SHAd1badcb64b64be828c97eaba5b03cf0f0e091168afce44329f9cbc498edc78a5 exact producteur.
Tous les fichiers A1A–A1H re-hachés inchangés. Pas60s ni tous les mots FIFO natifs.

La comparaison des alimentations s'arrête au premier échec, conformément au plan :
B02, les nouveaux paliers CN15 et le10min ne sont pas lancés. Le problème de compteur
existe aussi sous ST-LINK seul avec GDB présent ; CN15 et absence du débogueur ne
sont pas des conditions nécessaires. Cause physique exclusive nonétablie.
État utilisateur final :001hors tension/512Go auPC/JP6STLK/JP2retiré/CN15off.

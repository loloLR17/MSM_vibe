# Récupération001 C29 et correction de cible

Le signalement initial ducoffret rouge était libellé002 ; les premièreslectures ont
sélectionné explicitementSN002/UID002 et décrivent donc réellement002, pas001.
Utilisateurcorrige ensuite : coffretTR2-001/carte512Go, déjàcoupé pour le déplacer.
Aucune lecture002 ne doit être imputéeà001. Aucunreset/flash/START pendantrécupérations.

002 : erreur3 après2855,970262sMCU (47min35,97s),résidu131µs>100,SD/FIFOerror0,
STOPpropre/writerinactifnonincertain,4clips,7605527sortiescalculées. RAM786432B et
producteurSHA2145aec1058fe502025df07da0640ea3fea390366f30c63733b5ee68c079b4fc sauvés.
Deuxrecords enfile(156XYZ)/CRCvalides +59XYZpartiels =215XYZRAM préservés ; SDattendus
7605312XYZ àvérifier lorscopiecarte32Go, nonencorerécupérée. Le montage002avait été
signalédétaché/nonexploitable, ce constat n'est pasréattribué sansprécisionutilisateur.
Pasde nouvelessai ni gardeaffaiblie. TémoinnatifPID15364 déjàterminé lorscleanup.

001 : aprèscoupureuser, BIN C29 exact/SN001/UID001, nouveauboot99/error1/WHO0/run0,
SDnoninitialisée/0writes, aucun snapshotvalide. Les compteursnouveauboot etpayloadRAM
aléatoire ne décrivent PAS lacampagne antérieure ; causehistorique perdue, aucune
référenceRAM indépendante disponible. GDBdétaché/FIFOwriterinactifs/CShaute avant
transfert512PCuser, puis aucuneactionhardware jusqu'ànouvelordre de diagnostic.

CopieC290005.TR2 exclusive671088640B/native_sourceSHA=destinationSHA :
f7ca815f74b78c9e6a90f29c4ce26d5bc8f4c99e2b810fba10b1df82f1fc1ade.
En-têtev3/CRCvalide/source365b0ccf/UID001/CID512/FS16/488µgLSB/duréeprogrammée7200s.
LecteurstrictINCOMPLETE :0record/0XYZ/pasfooter. Scanintégral : aucune magieD3BK,
**TOUS les671088128octets aprèsen-tête512 sont nuls**, donc aucunblocXYZ récupérable.
Ce n'est pas une série de mesuresnulles : unrecord aurait toujours un en-têteD3BK.

ReprésentationGitHublossless : C290005.header.bin +671088128zéros, taille/hash décrits
parC29-lossless-representation.json. SHA de cettereconstruction calculé =SHA de la
vraiecopie ; le fichieroriginal complet reste localPC-copy, pas remplacé par dérivé.
H2A/T10 existent chacun640MiB maisen-têtes/queues512Bzéro ; seuls cesextrémités ontété
lues, pasdescanintégral/dequalification ni copiecomplète revendiqués pourcesdeuxfichiers.
Ils restent intacts surSD. Préallocationexplique tailleségales, pasduréeenregistrée.

C29prouvepréparationfichier/en-têteécrit, pasune capture de2h. Le trajetappelant
préparel'en-têteaprèsUSER et avantFIFO ; le point exact dudéfaut est inconnu sansRAM.
La cause de l'absence de données n'est pasattribuée à la coupure ouauWHO0du nouveauboot.
B02minute CN15+SD COMPLETE etD07D08 restent les seuls acquiscourts correspondants.

Suiteexplicitementautoriséeuser : reproductionGDB sur001, CN15conservé/STARTUSER.
GI0 nouveauGI00005.TR2 librePC, même durée7200s/préallocation640MiB/FS16/SPI3 8MHz/SD4MHz,
codefonctionnel inchangé. Imagechargée/vérifiée(source171cefc3/SHAe4c3f044…bb0ce).
Lecteurvide pourflash, bootsansSD99/error2/WHO7B attendu,pasSTART ; carte512toujoursPC.
Aprèsinsertionhors tension/CN15+STLINKmaintenus,suiviUSER/préparation/FIFO/terminal,
points d'arrêtavantFIFO ouaprèsSTOP, pasdehalts périodiques pendantacquisition.
AucunnouveaudépartavantprêtGDB etappuiUSER ; conserverlesanciensfichiers/gardes.

# T10 — refus de nom existant confirmé, fichier à préserver

Utilisateur rapporte RESETaccidentel avantUSER, puis rouge aprèsquelquesminutes
sansnouvelappuiUSER et reconnexionSTLINK sousCN15 maintenu. Aucun reset/flash/STARTagent.
SN/UID001 et BIN53340B/SHA8c4c12de…b0948 entier exacts, source d7bd4df0.

Premier relevé : bootinit(stage2/error0/run0/standalone1), WHO7B/FS04/cal41µs,
SD512CID exact/999743488secteurs, lectureFATencours/28871reads/0writes/0SDerror.
Aucun snapshotterminalvalide : magic0/CRCnonconforme. Les valeurs aléatoires imprimées
de retained.payload dans le log04 sont ININTERPRÉTABLES et ne constituent aucunrésultat.
Le rouge antérieur n'est pas directement conservé ; reconnexion suivie d'un nouveauboot
compatibleavec historique, causeexclusive nonétablie, CSRcumulé1c004400.

Observationparwatchpointmatériel surstandalone_state, aucunSTART/reset : finprécontrôle
stage99/error1, durable_status.error8, mounted1/formatted0, free_clusters15596052,
required20480, IOerror/uncertain/writes0, SDerror0, FIFO/writeroff/CShaute/GDBdétaché.
Code8 est le **code interne de contrôle du nom**, pas le résultat brut FatFsFR_EXIST.
Le code refuse tout f_stat différent deFR_NO_FILE ; sa seulevaleur8 n'aurait pas suffi
àprouverunfichierexistant (précision parrapportaux libellés historiques B02).

Vérificationciblée f_stat enlectureseule, scratchRAM512B sauvegardé/restauréidentique,
aucuneécriturefichier : FR_OK0, fnameT100005.TR2, taille671088640B/640MiB, attribut32.
La cause du refus actuel est ainsi confirmée : nomdéjàprésent, protectioncontreécrasement.
Aucune garde affaiblie. L'espacelibre est largement suffisant et aucun défautSD/FIFO
n'est observé dans ce boot. Cela ne conclut pas l'état d'une éventuelle acquisitionantérieure.

Le fichier était absent aucontrôlePC avantimageT10 ; sa taille correspond àlapréallocation.
Le code crée/alloue àlapréparation aprèsdépart, avantFIFO. Une tentativeantérieure
interrompue pourrait l'expliquer ; le gestephysiqueeffectif, lesinstants etles données
ne sont pas établis par ces seuls relevés. Le témoignage utilisateur est conservé,
sans l'infirmer ni considérer10min validées ou échouées pendantenregistrement.

Prochaineaction : transfert512GoauPC horsalimentationuser, copieintégrale exclusive
avecSHA, puis lecteurstrict/inspectionheader-prefix-footer, ycomprissi headerabsent.
Préserver640MiB etanciennescaptures ; aucune suppression/réparation/formatage/réutilisation.
Au besoin un nouveaunom sera préparé après récupération, pas d'essaiautomatique.
001SDquiescente etGDBdétaché, attenteconfirmationPC ; aucun002/FRAM/COM4.
B02minute autonomeCN15+SD COMPLETE reste acquise, D07/D08 aussi. Pas8hREADY.

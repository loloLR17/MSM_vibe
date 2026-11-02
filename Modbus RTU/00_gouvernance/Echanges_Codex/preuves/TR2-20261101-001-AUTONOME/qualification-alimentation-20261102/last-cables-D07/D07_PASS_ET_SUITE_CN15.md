# D07 — PASS 60 secondes, nouveaux câbles sur TR2-001

Utilisateur confirme remplacement des câbles puis vert fixe à la fin. ModuleB,
implantationbreadboard et brochage corrigé conservés selon les instructions.
Le détail des câbles/longueurs/mesures électriques n'est pas instrumenté.
Clarification historique utilisateur : l'erreur ayant produit WHO00 après déplacement
était CN8-6/8/10/12 au lieu de CN8-4/6/8/10. Ceci est distinct des refus compteur
D01/D04/D06 ; la correction avait déjà rétabli WHO7B et calibration dansD06.

Précontrôle001 : UID/SN corrects, BIN D01 entier50912B identique,
sourceacb142794570ded5abd15f6521eb5d8b644590e8,
SHA6a00ec329c3b16cbf1c01fc3661014ad6f1fed05732a60fecbc338d6a7bb98d2.
WHO7B/FS04±16g/SPI3 8MHz/calibration39µs≤100, WAIT. Aucun flash/reset/build.
Observer armé auWAIT, USERphysique puis CPU libre jusqu'au breakpoint aprèsSTOP.
Un seul essai, durée60000587µsMCU, stage4/error0,1585320natifs/158532sortiescalculées,
599checks/résidumax24µs≤100 ;0FIFO/stop_error/rails/clips/flags/queue_overflows,
budgetmask0. Maxima complets service702,49375µs, consumer373,75µs, gap961µs,
appelwriter vide1,01875µs. Le dernier n'est pas une mesure de transfertSD.
SnapshotCRC1779fd5f/sourceacb conformes ; SHA dans validation.json.

Relevé07 confirme FIFOstoppedtrue, writerinactive, commands/reads/writesSD0,
initialized/write_enabledfalse, CS haute, GDBdétaché. Pas de fichierSD créé ; sorties
calculées et digest conservés, pas d'archive complète XYZ. Aucun002/FRAM/COM4.
Validation physique bornée60s du capteur/FIFO sur001 sousSTLINK ; aucune qualification
stockage/endurance/alimentationCN15 acquise. Pas de test ancien/build relancé.

Le succès après remplacement des fils est compatible avec un problème de liaison
ou contacts ; pas de mesure électrique ni de fil précisément identifié comme défectueux.
Ne pas condamner MCU ou capteur ni annoncer une cause exclusive.

Après ce succès, utilisateur demande explicitement de poursuivre vers CN15 et
l'autonomie au réducteur : cela remplace son intention antérieure d'arrêter après
le dernier test. Prochaine tranche : même image D01,60s sousCN15 seul/alimentation
secteur5V stable, sansSD ni flash. Aucun nouvel essaiSTLINK inchangé.
Les gestes JP6USB7–8/JP2retiré/sourceexterne/stlinkoff se font uniquement hors tension
par utilisateur ; attendreWAIT, départUSERphysique, conserverCN15aprèsSTOP pourrelevé.
Ensuite seulement, reprendre chaîne enregistrementSD/fichierneuf/lecturePCintégrale,
puis palier10min si conforme. Ni powerbank ni8h ne sont encore qualifiés.

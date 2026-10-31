# Comparatif capteur de référence rapporté par utilisateur

À5minutesdudépart, utilisateurconfirmeéchangeducapteur avec celui deTR2-001. AucunaccèslogicielàTR2-001. Moduleidentifiésuivantcettedéclaration, WHO7B communauxIIS3DWB, référencescarte/identifiantunique nonfournis : nepasinventerd'identificationdistincte.

Aupréflight, mêmeUID002 maisRAMencore99/run1/error1 antérieur : pasdeWAITrebootobservé. Premiercontrôleetpremierappeldecalibration refusésavantcommandeparleursgardes, aucuneacquisition. ImageD53032B entièrementidentiqueauBIN889c551f…43cfe vérifiée. Arrêtsûrcontrôlé puisunresetlogicielnécessairepourréarmer, sansflash/build. Repriseinit/calibrationuniquedepuisWAIT.

Résultat : WHO7B/6577ancresFIFO/198brackets,0FIFO/SDerror, maisrésiducalibration1875862µs>100 ; error7/99/run1. PasdegateSD/pasnouveaufichier/pasSTART8h. La gardesestrictes sontconservées. Symptômerécidiveaveclemodulederéférence : n'isolepasledéfautcommeproprespremiercapteur, n'exclutpastransport/liaison/conditionscommunes, aucunecausalitéélectriquecertaine.

Finalvérifié : FIFOoff/stoperror0/writeroff/clockfault0/CShaute/GDBdétaché, imageDduplex8MHz/HIGHconservée, READYfalse. Autorisationphysiqueuserlimitéecapteur, autresdonnées/firmware/FRAMde001nonaccédés. Preuvessanspertezlib/manifestSHA. Aucun nouveauflash/resetautomatique/long autoriséimplicite.

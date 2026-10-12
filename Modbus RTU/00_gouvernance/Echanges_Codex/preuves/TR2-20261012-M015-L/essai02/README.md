# M015-L — essai n°2 après correction du câblage SPI

Autorisation explicite utilisateur de reprendre après correction. Cette sous-arborescence sépare la campagne n°2 du premier échecWHO00, dont toutes les preuves du dossier parent sont inchangées. Rapport essai1 archivé exact avant reprise.

FreshUID/SNseconde reconfirmés, Flash2MiB doublelecture fraîche identique et identique à restauration essai1. Artefacts K inchangés, aucunbuild outestlogiciel rejoué. WHO essai2unique =7B/result0/cleanup0/trace1/aucunwritecapteur. Polling ST =64XYZ réels distincts/0saturation/309tracesHAL_OK ; CSV64 reconstruite exactement au buffer RAM.

FIFO strict : ST_LEVEL8, API0/statut brut suivant257, getterST03F maintenu, 840tracesHAL_OK/0XYZ. Variante séparée STATUS10 selonprotocole, réglagesST inchangés : premierburst128 demandé/896octets, trace834 HAL_TIMEOUT3/error104=OVRSPI+TIMEOUT ;841traces total, cleanup0, fenêtre0. Les flags FIFOcapteur observés avantlecture ne signalent pasoverrun ; aucunstatutaprèsburstéchoué, pas qualificationd'une absenceglobale deperte.

Buffer de896octets préinitialisé àzéro, partiellementmodifié ; taille réelle reçue nonenregistrée. HAL_CloseTransfer remetTx/RxXferCount0 mêmeéchec. Hexpublié =bufferrejeté, pas128XYZréels ; slotszerosnonmesurés exclusde touteacceptation. Commandesreconstructibles parreg/sens, RXcommande noncapturé, paswirecapture.

SortieXLoff/bypass/timestampoff commandée parcallbacksHAL_OK. SeulssecteursFlash0..3restaurésdepuisbackupfrais, hors32KiB exactavantrestauration ; Flash2MiB entièrebyte-identique/SHAfcea004e…/UIDexact/faults0/detach/serveursexit0. Firmwareinitial nonqualifiéfonctionnellement. TR2principal/M014-H jamaisaccédés/COM7/médiasintacts.

LogsCLI/GDB complets normalisésUTF8/LF, CSVLF ; manifests relientpublié/original. OriginauxFlash/RAM/BIN/ELF/scripts : /home/lolo/dev/msm/reports/TR2-mission015-L-attempt02-20261012T201926Z. Paramètres effectifs sourcee222dc8/STmode0/core16MHz/SPI2MHzcalculés/DebugsansO ; cecontexteest différentdeI/J. PasnouveaurunaprèsrefusC2.

Prochainevérification logicielle : capturerSPI_SR etnombrelu avantSPI_CloseTransfer, mesurerbudgetdeserviceRXà4µs/octet pourHALReceive896, vérifierclock/optimisation/packingRXduSPI3U5 sanschangerODR/FS/batchingST ni contoursd'erreur. SaturationCPU esthypothèse, overrunMCU estfaitHAL ; aucunefautecapteur attribuée. WHOetpolling validésdanscetteportée, FIFOlongnonqualifié.

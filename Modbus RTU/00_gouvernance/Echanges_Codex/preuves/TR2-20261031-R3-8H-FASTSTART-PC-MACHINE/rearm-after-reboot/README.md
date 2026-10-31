# Réarmement après redémarrage — échecs conservés séparément

N8A historique conservé. Deux images issues du même code75ea886c, seuls préfixe et provenance diffèrent du profil éprouvé. BINN8B91b19122…225c3 et BINN8C0844bc7a…2009a, builds ciblés réussis/relecturesFlash entières exactes. Aucun algorithme ni garde modifiés.

- **N8B** : init/carte/calibration réussies, noms0003/5 absents puis création du seulgate0003. Arrêt0,205253s : error5/fifo9/parserreason1 (paritétag),5168natifs/517sorties/0rails0clips/FIFO33,0SDgroupacquitté. File partielle enRAM3072B ; header/invalidfooterSD présents, pasdefooterCOMPLETE. Rafale fautive extraite : premiers4tags paritévalide puis tags incohérents/décalage apparent, causeélectrique nondémontrée. FichierN8B0003.TR2 incomplet àpréserver ; aucun long.
- **N8C**, unique reprise bornée justifiée par échecN8B : error2/fifo9/parserreason6 (tag nonreconnu) pendant calibration, WHO7B/carteinitialisée mais0sortie/0writecapture. PasdenouveaugateSD ni long. Sélection/compteurs de nomsC non atteints ; ne pas prétendre nomsC déjàvérifiésabsents.

État final contrôléGDB : stage99/run1/error2, FIFOoff/writeroff/card.failedfalse/clockfault0/CShaute, debuggerdétaché. L'utilisateur peut couper le ST-LINK puis inspecter/réenficher les connecteurs capteur hors tension, sanschangerbroches/pose ni toucherSD. Ne pasrépéteraveuglément lesflashs/tests ; causephysiqueexactenonétablie. Le firmwareC resteinstallé et pourra être réutilisé aprèsintervention, sansflash si relecture/état conformes. Au retour, nouveaux noms de traces pourpréserver celles-ci.

Les sorties et dumps binaires bruts sont compressés sansperte ; MANIFEST.json permet vérification aprèszlib. Le premier appelhelperB a échoué avanttoutSTART car native-host.json local manquait ; lecturehôte faite ensuite puis uniquegatephysiqueB. Les deuxéchecsphysiques etl'erreurdepréparation restentdistincts. Leshelpersarchivés sont spécifiques auxcheminslocaux, pas des commandesàrelancer depuisce dossier.

AucunSTART8h, aucunCOM4/TR2-001/FRAM/formatage/protectionWindows/OptionBytes. Les anciennesvalidationsréussies n'ontpas étérejouées ; buildARM de chaque nouvelleimage etuneseulereprise de validationéchouée seulement. Inspectionconnecteur indispensableavanttoutecapturelongue.

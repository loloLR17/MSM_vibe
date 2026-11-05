---
mission_id: TR2-20261105-SHIELD-V21-FIXATIONS-FINALES
status: IN_PROGRESS
base_ref: main
base_sha: c23f5e0109d53a0aaad984fcff16399985b6ab24
initial_head: e8f240ba766ed5ce506b0c467772490ee0d589ad
result_sha: ea1ce5507d9386d7dfa4014d1c26162bb539c280
created_at_utc: 2026-11-05T12:52:44.429738+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.1 — D126 retenu pour prototype ; fixation réelle non qualifiée

**Candidatprioritaire dimensionné : trois supports isolantsD126 en demi-couronne, G12,25±0,10, visnylonM2×20 et deuxécrousnyloncontre-bloqués au-dessus du shield. GO géométrique sousbornes ; tenue/fabrication/approvisionnement dusupport BLOCKED, libérationPCB NO-GO.** L’étude est terminée, publication/relecture en cours. **La fixation n’est pas définitivement qualifiée physiquement.**

[Décision et comparaison](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/DECISION.md), [fiche de montage](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/ASSEMBLAGE.md), [approvisionnement](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/APPROVISIONNEMENT.md), [plan/coupe](../../04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/cad/plan-et-coupe.pdf). STEP/SCADéditables et STLderevue, scripts etpreuves dans `Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_1_FIXATIONS/`.

## Choix et dimensionnement

Troissolutions comparées : tube isolantmince standard+calage (jeuvis/longueur/diamètre nonmaîtrisés), profilouvertD126, repriseparchâssis (plusdepiècesetnouvellescotescoffret). **D126retenu sans nouvelle électronique** : enveloppeØ6, alésage2,4, hauteur12,25 ; demi-couronneouverteàl’axe sansminceparoi avant, ligamentradial1,8, section11,875mm². OuvertureversCN15àl’ancragehaut, versSTLINKauxdeuxbas, rotationmaxacceptée±5°. Sixpiècespourdeuxprototypes+deuxréserves. FabricationPETGdense deprototype proposée ; mêmesSTEPpourusinagePA66/SLS-PA12siaccessible, aucune substitutionmatière déclaréevalidée.

Unpremier profilØ6/méplat1mm orientépareilpartout croise deuxsolides duheaderST près del’ancragehaut : essai **rejeté**, preuvesconservées. Le profilfinalretourneau pointhaut et dégagelespartiesporteuses. Une pièceouverte ne doitpas être confondue avecun troufiletéamputé : vistraversanteetécrousassurentleserrage, le support ne reçoitpas defiletage.

## Tolérances réconciliées et contrôles

L’écart nominalaxe→bordZioST est2,285mm. Le déplacementdusupport n’estpas réduit au budget0,15mmV20 : il dépenddesdeuxtrous, du diamètredetige etdujeu danslesupport. Budgetexplicite : `min((3,33−1,90)/2 ; (2,93−1,90)/2+0,15)+(2,60−1,90)/2 =1,015mm`. TroumaxNucleo3,33/shield2,93 et écartcentres0,15 sontdesbornesd’acceptation, nonmesurées ici. Inclureseparément positionancrage→Zio0,075, demi-tolérancelongueurcorps0,125 etbavure0,38 ; nepas soustrairedeuxfoislamêmeforme.

AvecprofilRmax3,10, faceenavantmax0,10, rotation5°, projectionavantmax0,369662 ; **margeplane2,285−0,125−0,380−0,075−1,015−0,369662=0,320338mm**, supérieureàlagarde0,20 souscesbornes. **Pas de garantie touslots** : la fabrication/pose doivent respecterlesbornes ; résistanceetmaintienangulaire nonqualifiés. LesinégalitésG/J5V20 restent réutilisées, I5=4,15..4,81 etIz=4,00..5,72 soushypothèses. Aucune modification des106centresXY niGparajoutdecalescachées.

Essais réellementexécutés avec CadQuery/OCCT : **9cas supports** (3ancres×nominal/rotation±5°/translation1,015versleheader, profilauxbornes) contre1060solidesSTC05, aucuneinterférence. Ce n’estpasunbalayagecontinu exhaustif detouteslestranslations. CacheBREP/sourceSTV20 etcoordonnéesV1.9 réutilisés, sansrépéterERC/DRC/firmware.

Visserie nominale modélisée : DIN84PA6.6M2×20 têteØ3,8/haut1,3 **sousNucleo**, deuxécrousDIN934PA M2 au-dessusshield ; clé4, coinsØ4,619, pasdesécrousprésumésØ4,5. PasderondelleØ5standard ajoutée, pasdemétalconducteuraucontactcuivre. Stack18,65 laisse1,35mm de vis ; têteZ−2,9, sommetécrous17,05, vis18,4, bottommodules24,85. Cotesréellesdu lotàconfronterauxspécificationsd’acceptation, pas àdesdessinsdevignette.

**Aucunecollisionimprévue de visserie nominale**. Troisrecouvrementsmesurés tête/PCB0 ont chacunvolume1,458e-5mm³ et profondeur4,42nm surlafacedeportéeZ−1,6 : contactsintentionnelsdetolérancenumérique, conservésetclassésparleurprofondeur/localisation, sansaugmenter globalementleseuildecollision. TroisSTEProuvertsvalides ; STLfermé/volume positif ; PDF1page rouvert ettextecontrôlé ; JSON/Pythonparsés. Commandes/scripts effectifs `final_model.py`, `fasteners.py`, `views.py`, `validate.py`, preuves `geometrie-et-limites.json`, `visserie-ST.json`, `validation-finale.json`.

## Tenue et disponibilité — limites réelles

Àl’effortcible5N/vis, moyenne0,421MPasursection/≈1,47MPasurlademi-portéesouslatête ; **pasunechargeadmissible prouvée, pasuncoupleprescrit**. Excentricité, flexion, impression/couches, friction/rotation, température, fluageetvibrations restentàqualifier. Contre-bloquerlesécrous sanscintrerla carte ni augmenter la précharge. Rotation±5° n’estpasgarantie paruneclavette ; elle devraêtrecontrôlée aprèsmontage etessaidetenue. Ne pas appelerunPETGfineparoi «robuste» parprincipe.

Approvisionnementvérifié surpagespubliques le05/11 : **SAM/ScrewsAndMore**, fabricant-distributeurallemand livrantFrance, deuxpacks25vis`84PA220_25`/25écrous`934PA2_9451`, chacun5,39EUR TTCaffichés, disponible sansquantitéentrepôtgarantie. Besoin8vis/16écrous avecréserve, lots25. PortEurope14,90TTC19% ; **25,68EURaffichés /25,90EURbudgétéTVA20%**, pasundevischeckoutFrance. Délaiannoncé1–3joursnonconfirmépourlaFrance ; nereprendpas lefrancoAllemagne. TouteslesURLs dansAPPROVISIONNEMENT.json. Aucunpanier, contactcommercialouachat.

**Supportsspéciaux : prestataire, stockmatière, prixetdélaiFranceNONVÉRIFIÉS.** Environ1,48gPETGpour8pièces, coûtbrut≈0,044EURà30EUR/kg : pasleprixd’unserviceimpression. Totalrenducomplet`null`, pasun lotcompletpromisà25,90EUR. L’exigenced’unesolutionlivrableetphysiquementrobuste n’est doncpas déclaréesatisfaite sansréserve. La piècese réaliseavecdesfichierssimplesfournis, maisaucunemachine/prestataire disponible n’est supposé acquis.

## Montage et décision

Pasdenouvelleimpressionpapier, pasdenouvellemesuredetouteslesbroches. Le contrôlefroiduniquedupremierexemplaire porteraitsurlesbornesduprofil, sonorientation, l’appui libre/G/parallelisme, l’engagementJ1–5, lesjeux/supports/têtes/soudures etlesaccès. Ilne lèvepasautomatiquementJ5/G, LED/vitrage,USER, isolationGND2oueffortsdecâbles. La ficheconserveUSERdirectsimple, RESETprotégé etlesinterfacesV1.9/V20.

**Lot5PCBnusenvisageable commeprototypes àrisquecirconscrit**, après revueindépendanteChatGPT et décisionfinancièredistincteutilisateur : fixationhorscuivreajustable mais fabrication/tenueetemboîtement peuventéchouer. **Aucunecommandeni libération parCodex.** Aucun gestephysique demandé maintenant, aucunflash/debug/COM/média/sécuritéWindows concerné. Unevérificationmatérielleultérieure demeureindispensable ; aucun coupleou montage forcé autorisé.

## Git et journal

Codeévalué **ea1ce5507d9386d7dfa4014d1c26162bb539c280 — Dimension V21 D126 isolation supports and verify mounting clearance cases**. HEADinitiale8f240ba, basecontratc23f5e01 ; worktree`MSM_vibe-SHIELD-V21-FIXATIONS`, branchelocale`codex/shield-v21-fixations`, ciblemain. **464fichiers historiques shield inchangés**, électronique/GerberV1.9préservés, ancienrapportV20/archivesintacts. Dépôtprincipalpropre82169af7/deuxstashes6251b988…/f3b829e6… etautresworktreespréservés, pasdeopérationGitdestructive.

Journal `ETAT_COURANT_TR2.md` actualisé : candidat, marges/essais, limitesphysiquesetdevis, prochaineétape. ÀtransmettreàChatGPT : reviewerlesbornesetlecompromismécanique ; aucunGO définitifdedisponibilité/tenue. Permissions exercées : étudeCAO/docs/calculs/commits/pushnormal ; aucunmatériel/achat/impression.

Publication et relecture distante : en cours. DONE visera l’étude livrée et ses réserves, pas une fixation physiquement qualifiée ou READY_TO_ORDER.

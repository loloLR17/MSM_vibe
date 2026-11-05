# V2.1 — candidat de fixation retenu D126, GO géométrique sous bornes

**Retenu pour premier prototype : trois supports isolants identiques en demi-couronne, G12,25, visserie nylon M2 traversante, deux écrous contrebloqués sur le dessus du shield.** SupportsD126 : diamètre enveloppe6mm, demi-couronne ouverte jusqu’àl’axe, alésage2,4, hauteur12,25±0,10après fabrication. Ouverture versleheader voisin : **versCN15 au point haut, versSTLINK aux deuxpoints bas**, flèche à maintenir dans±5°. Modèle[STEP](cad/D126-support.step), [SCADéditable](cad/D126-support.scad), [STLderevue](cad/D126-support.stl), [implantation](cad/implantation-3-supports.step). **NON LIBÉRÉS pour fabrication en série ni qualification mécanique réelle.**

L’ouverture supprime la mince paroi qui rendrait un simpleméplat fragile ; sectioncomprimée11,875mm², ligamentradial1,8mm surles partiesporteuses. Volume145,47mm³/support. Faceouverte au droit de l’axe, pas un trou fileté coupé ou un ressort. La vis guide le montage, **elle ne crée pas la hauteurG**. Sans contactstable sur les appuis, ne pas serrer pour compenser.

|Solution comparée|Constat|Choix|
|---|---|---|
|Tube nylonØ3..4 + rondelle/calage|Pièce standardéconomique, mais corps mince, jeu d’implantation vis+tube et bordZio ne sont pas maîtrisés ; G12standard+cale0,25 créeune interface et nécessite une cale trèsfine de diamètrecompatible.|Écartée comme solutiongarantie par seul diamètre/catalogue ; pas de référenceinventée12,25.|
|Demi-couronne D126 + vis nylonM2|Sectionpluslarge, face dégagée au lieu de réduire tout lediamètre ; GO géométrique sous bornes et contrôles ci-dessous. Une pièce par appui, six pourdeuxcartes.|**Candidat prioritaire**. ImpressionPETGdense locale deprototype ; usinagePA66 ouSLS-PA12 du mêmeSTEP si serviceaccessible. Prix/délaidefabrication spéciale nonconfirmés.|
|Reprise sur châssis/berceau du coffret|Peut immobiliser avecvis standard mais impose une nouvellepiècemultiappuis, cotesboîtier et contrôle du supportage/ports. Plus de transformationsque nécessaire.|Repli si D126 n’estpas réalisable/qualifiable ; aucune reconceptiondecoffret exécutée.|

## Risque local réellement identifié et correction

Les deuxancragesbas (33,89;97,39)/(79,61;97,39) sont prochesdes extrémitésCN9/CN10 ; distanceaxe→bordcorpsST2,285mm. L’ancragehaut(32,616;21,192131) est près d’unpetitheaderST surl’autre côté. Unpremier profil6mm/méplatà1mm, de mêmeorientationpartout, **croise deux solidesST au point haut** (preuvesconservées). Le profilfinal retire une demi-couronne et retourne le supporthaut : **aucun contact positif aux troisancres danslescas testés**. Pas de déplacementXY, trous ou électronique.

La bavureSamtec0,38 et demi-tolérancede longueur0,125 ne sont pas la même chose que le déplacement dusupport. Borne de déplacementlibre explicitée :

`D = min((3,33−1,90)/2 ; (2,93−1,90)/2+0,15) + (2,60−1,90)/2 = 1,015mm`.

Diamètresmaxde trous et demi-jeux sont des **budgets d’acceptation**, non des mesures certifiées des deuxcartes ; sourceSTtrounominal3,2 etshield2,8. Le secondterme représente le jeu du support sur la vis. Ne pas réduire arbitrairement ce déplacement à0,15mm comme un positionnementimposé. Profil fabriquéR≤3,10, faceavance≤0,10 et rotation≤5° : avancemaxverssocket0,369662mm. Jeu pirecasplanar :

`2,285−0,125−0,380−0,075−1,015−0,369662 = 0,320338mm > garde0,20`.

L’avance du profil contient déjà l’erreur deforme et la rotation ; elles ne sontpas soustraites unedeuxièmefois. Toléranceancrage→socket0,075 et différence entre trousdescartes0,15 sont distinctes, mais hypothétiques et àcontrôler ; **pas de garantieindustrielle sur tousles exemplaires**. Écartsangulaires descontactsSamtec ne sontpas une translationde mêmeamplitude ducorps. Les9casCAO sont troispositions×nominal/rotation±5°/translation1,015 versleheader, nonun balayageexhaustif detouslesassemblages.

**G et emboîtement ne sontpas requalifiés** :12,25±0,10 avecappuisbienplans conserve les budgetsV20, I5=4,15..4,81, Iz=4,00..5,72. Aucunintercalaire ajouté entresupport etPCB, sinon ajoutersonépaisseur àG ; dessus/dessousdesPCBs restent définis commeV20. L’incertitudeCN12 et les contrepartiesexactes restent ungate séparé.

## Visserie et contraintes mécaniques

VisDIN84PA6.6M2×20, têtecylindriqueØ3,8/haut1,3 sousNucleo ; deuxécrousDIN934PA M2 dehauteurmaxacceptée1,6 chacun **surledessusshield**. Clé4mm, coinsØ4,619 : **lesécrous nesontpas réputésØ4,5**. Pas derondellelarge. Leuremplacementaudessus et leurnatureisolante évitentd’imposer une grandepièceconductrice sousNucleo. Pas de fraisurage d’un PCBni de visacier aucontactcuivre.

Stacknominal souslatête :PCB1,6+support12,25+PCB1,6+2écrous1,6=18,65mm ; vis20laisse1,35au-dessus. TêtesousNucleoZ−2,90 ; sommetdesécrous17,05, vis18,40 ; modulesbottom24,85, marge6,45àlapointe danscemodèle. AvecvisL≥19,8/G≤12,35/PCB≤1,76/écrous≤1,6, débordement≥0,73mm. Cesvaleurs sontspécificationsd’acceptation, pas desdimensionsmesurées dunlot. Lescontre-écrous ne doiventpas modifierG nicintrerlePCB ; on bloque lesécrousentre euxenmaintenant laposition.

**Résistance/fluage NONQUALIFIÉS** : PETGprintéàplat, plein, paroiscontinuessur11,875mm², ligaments1,8mm : pas deparoi0,1mm ni clipflexible. Pourun effortcible5N/vis, contrainte moyennesection0,421MPa ; **5N estunobjectif de calcul, pas une capacité prouvée niuncoupleprescrit**. La surface réellement chargée sous une têteØ3,8 n’est que π(1,9²−1,2²)/2≈3,41mm², soit≈1,47MPa à5N ; ce calcul est encore une moyenne, pas une contrainte locale maximale. Contactdemi-face, excentricité, frictionderotation, filament, couches,température, vieillissementetvibrations empêchentd’en déduire uneprécharge sûre. Pasde serrageautomatique au coupleinventé. Le maintien±5° dépend dumontage/contreblocage, pasd’une clavette démontrée. Contrôlefroiddupremierexemplaire puis essaidetenue nécessaire ; matériauPA66usiné/PA12SLS peutêtrepréféré siPETGneconvientpas, sans présenter une substitution comme déjàvalidée.

## Décision et portée

**GO géométrique sous les bornes décrites pour D126 ; BLOCKED physique/résistance et disponibilitédu supportspécial ; NO-GO de libérationPCB.** La fixation est dimensionnée et un candidat clair est retenu, **pas une fixationdéfinitivementqualifiée**. Aucun autreplacementélectronique modifié. L’étude de visserie ne lève pas les réservesJ5/G, fenêtreLED/USER/câbles ni l’isolationGND2.

Les5PCB nus peuvent raisonnablementêtre envisagés comme **lot deprototypes àrisque circonscrit** après revueChatGPT et **décisionexpresseutilisateur distincte** : XYconservé, supporthorscuivreajustable, mais échec defixation/engagementpeutencorenécessiter une reprise. Ce n’estniuneautorisationdecommande niunequalificationdécoulantd’unachat. Si aucunserviceouimprimeurn’estdisponible, nepas masquerle coûtdudélai parun prixdefilament. Aucun papier supplémentaire demandé.

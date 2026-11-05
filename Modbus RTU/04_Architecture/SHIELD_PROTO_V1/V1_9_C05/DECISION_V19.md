# V1.9 — correction XY C05 démontrée, fabrication toujours NO-GO

**Un défaut de placement précis est corrigé : CN7/J1 était trop haut de1,016mm.** En gardant l’ancrageCN8/J2, CN9/J3 etCN10/J4 sont cohérents avec la CAO ST ; aucun déplacement général de ces connecteurs n’est justifié. J5 est ajusté de+0,025001mm enY, sans changement de format ni de branchement. Un centre de fixation bouge de−0,004mm enX/+0,002131enY, moins de0,005mm. Les deux autres fixations sont inchangées. Le «plusieursmillimètres sur certainsconnecteurs» observé par l’utilisateur reste un constat qualitatif : **nous ne prétendons pas avoir reproduit/attribué toute l’ampleur observée sur papier**.

[Comparaison106broches avant/après/ST](comparaison-C05.csv), [cinq trous ST et trois ancrages utilisés](comparaison-trous-C05.csv), [repère/deltas](preuves/comparaison-synthese.json), [provenance](PROVENANCE.md).

## Démonstration indépendante du repère

ST natif dessus :Xdroite/Yhaut. Shield dessus :Xdroite/Ybas. AncrageCN8-1→J2-1(32,62;35,16), transformation `Xshield=XST+21,74999888 ; Yshield=126,60113030−YST`. Aucun miroir de montage : les connecteurs sont sous le shield mais les axesXY restent ceux de la superposition dessus.

Avant alignement global, distances relatives CN7-1/CN8-1 : **ΔX45,72mm** dans les deux jeux ; ΔY/ST9,144mm contre10,16mm dans le shield. C’est donc un placement relatif erroné deCN7, **pas une simple translation rigide de tout le plan**, ni une erreur d’échelle de1%. Tous les106centres de contact du candidat correspondent ensuite àST à≤0,000002mm numériques. Cette précision de calcul n’est ni une tolérance fabricant ni une certitude d’emboîtement physique.

## Correction minimale et contrôles

Les originaux V1.4–V1.8 restent intacts ; candidat autonome éditable [KiCad](cad/TR2_Shield_Proto_V1.kicad_pro), [PCB](cad/TR2_Shield_Proto_V1.kicad_pcb). Schéma, règles de projet, matrice électrique, modules, contour90×100 et fixationsMKI/Click **inchangés**. J5Samtec femelle2×3 sous shield conserve ses cinqNC distincts, seul local5/global69/PG4→DE, mâles existants66/67/68/69 et65/70vides. Aucun changement deNucleo.

Six fins de pistesCN7 recalculées localement, leurs préfixes conservés ; finRTU_DE décalée25µm pour garder la marge auNCvoisinJ5. **25des32routes sont identiques**, dont les quatre du capteur. Pas de déplacement automatique des modules pour absorber l’erreur. Le premier routage a réellement produit des violations ; contrainte de couche au raccord des préfixes et décalage cohérent de la finDE corrigés, **sans changer les règles**. Résultat final : **ERC0/DRC0/non-connectés0/parité0**, quatrevias,19empreintes/191pads. Voir [validation](preuves/validation-finale.json) et `docs/checks/`.

Netlist et X2cuivre comparés aux nets réels ; centres/diamètresExcellon vérifiés un-à-un, erreur maximale de quantification0,000131mm pour une résolution de sortie1µm. La première sonde exigeant quatre décimales à un fichierExcellon à trois a échoué ; sa correction compare la résolution du format, **sans arrondir les coordonnéesCAO natives**. Aucun contrôle physique n’est déduit du DRC.

## Portes séparées

|Porte|État et suite minimale|
|---|---|
|XY C05 officiel|**Concordance CAO nominale démontrée**, sourceC/05 identifiée ; nouveau papier sur cartes réelles encore requis. L’ancien contrôle papier ne qualifie pasV1.9.|
|Z/J5/G|**NON VÉRIFIÉ** : dimensions acquises pointe8,22/corps2,40 restent ; corpsJ5H8,51 et Gnaturel/tolérances à confronter, ne pas adopterG11 automatiquement. Ni le papier niSTEP ne mesurent l’engagement réel.|
|Fixations/modules|Trois centresNucleo recoupés ; trousshield2,8 face trousNucleoST3,2, fixations à choisir sans contrainte. RetenuesMKI/Click inchangées, révision et appuis/isolement réels restent à confronter.|
|USER/RESET|B1origineST→(29,5212;120,3262), B2→(84,2074;120,3262), hors contourshieldY117. **Accès direct protégé** toujours prioritaire, aucun mécanisme complexe. Hauteur capuchon10,2/course0,4 acquises ne sont pas remplacées par la hauteur de corpsCAO7,55.|
|LED/vitrage|OriginesempreintesLD3/LD2/LD1=(51,72/54,92/58,12;20,92). Ces données remplacent les inconnuesXY, **pas des centres lumineux mesurés**. Les rayonsV1.7 fondés sur un domaine hypothétique ne qualifient pas ces trois positions ; vitrage16×8 ancien non figé. Pas de découpePCB justifiée ; implantation d’une fenêtre latéralePC à finaliser aprèsG et visibilité réelle commune.|
|CN1STLINK/CN15|OriginesST dans le repère :(70,716;−6,706871)/(67,02;126,62), aux extrémités extérieures du shield. Fiches réelles/câbles et hauteurs non certifiés, ajustements de coffret après compatibilité.|
|SD/RS485/nappe|Modules et sorties inchangés ; pas de nouveau conflitXY démontré par cette correction. Enveloppes/appuis/fiches du vraiempilage toujours à contrôler ; pas de qualification d’accès par une vue seule.|
|Fabrication|**NO-GO** : CAM neuf [ZIP NON LIBÉRÉ](exports/TR2_SHIELD_V19_NON_LIBERE.zip), [manifeste](exports/CAM-manifeste.json). Aucun envoi fabricant ni commande.|

## Un seul contrôle utilisateur ciblé

[Gabarits corrigés, deuxpagesA4](exports/GABARITS_C05_V19_100POURCENT.pdf) : vue dessus, sans miroir, impression100% et deux barresX/Y100mm. Chaque page est associée à une C05 ; ne pas prétendre que les deux cartes ont déjà été contrôlées. Banc concerné hors tension, sans déplacer les fils ; confronter les cinqinterfaces et trois centres de fixation, sans forcer. Retour unique : barresX/Y, carte, centres/fixations OK ou lieu du décalage. Si un écart reste, une photo orthogonale du papier suffit pour le localiser ; **pas de relevé manuel de toutes les broches**.

En parallèle seulement si connecteurs témoins exacts déjà disponibles : noterG naturel et jeux, sans assemblage forcé/achat improvisé. Si inaccessible ou non mesurable, le noter. La preuve XY et la preuve Z restent distinctes ; aucune nouvelle demande des cotes déjà acquises. Fabrication uniquement après ces résultats et revue indépendanteChatGPT/décisionhumaine.

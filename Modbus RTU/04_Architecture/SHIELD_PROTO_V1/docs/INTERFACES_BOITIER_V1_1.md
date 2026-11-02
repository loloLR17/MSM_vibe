# V1.1 — interfaces du futur boîtier, sans boîtier complet

**BLOCKED_FABRICATION.** Plan1:1 `exports/interfaces-boitier.svg` et PDF associé ; positions numériques dans `mechanical/interfaces.json`. Axes du PCB KiCad : X droite, Y bas, Z au-dessus du support ; origineZ = face supérieure du shield. Les zones en pointillés sont des réservations fonctionnelles, **pas des découpes libérées**. Aucune mesure réelle C05 ni usinage n’a été effectué.

## Accès microSD effectivement étudié

La fiche du socket **4UCON15882**, fournie par Adafruit1660 et reliée à l’empreinte Eagle `MICROSD`, identifie un socket push-push. Sa vue d’éjection confirme la sortie par le bord courbe, opposé au bord des broches du breakout. La rotationR180 dans le PCB Adafruit, puis la transformation d’implantation du shield, donnent une sortie **-X**. Le sens V1 était donc correct ; le premier examen de photo seul a été rectifié par cette fiche avant toute modification d’orientation.

J7 est déplacé de(44;106) à **(40,5;106)mm**, sans rotation ni changement de net. Corps25,4×22,86 transformé : X20,18..43,04 / Y83,14..108,54. Ancrages du module : **(22,72;85,68) et(22,72;106)**, sans étrier au-dessus de la fente. Le pointage GPIO reste SPI2PB13/PD3/PD4/PF12. Les broches J7 se trouvent à droite du module ; le retrait se fait vers la gauche.

Le profil courbe du socket et le modèle3D fabricant permettent de réserver une **bande nominale de bouche X21,1..23,1 / Y88,8..103,0mm**, avec hauteur provisoireZ12..14,5mm sous l’hypothèse module à11mm. Ce n’est pas la cote métrologique du module réellement détenu : elle reste à contrôler avec l’engagement des sockets. La fiche donne des dépassements de carte **0,60mm poussée,1,60mm verrouillée,5,30mm éjectée**, mesurés suivant ses datums. Ce sont des positions de dépassement, pas trois courses à additionner. Différences : verrouillé→poussé1,00mm, verrouillé→éjecté3,70mm.

Paroi externe gauche proposéeX16mm : le bord de la carte éjectée pourrait rester légèrement en retrait. Une simple fente mince serait donc insuffisante. Le plan réserve une **poche accessible aux doigts X0..27 / Y83..109 / Z10..21mm**, qui permet d’atteindre la carte dans cette plage de bouche. Le volume de retrait complet est dégagé vers-X ; la trajectoire de la carte elle-même est contrôlée contre les têtes de vis, avec une allowance de tête5mm. Les vis restent latérales à la carte ; la poche peut les rendre visibles sans imposer leur démontage. Contrôle nominal seulement, pas essai d’ergonomie réelle.

Le futur boîtier doit prévoir une trappe/capuchon simple démontable devant cette poche : protection poussière et prévention d’appui involontaire sur le push-push. Aucun mécanisme sophistiqué ni étanchéité annoncée. La trappe ouverte doit laisser tout le débattement prévu ; ne jamais remplacer l’accès latéral par le démontage du breakout.

## RS485 extérieur et isolation

Connecteur du Click inchangé, long côté isolé vers+X. Sur le CAD **v101**, centres des deux borniers : environ(137,385;86,731) et(137,385;97,399)mm ; contour terminal de module jusqu’àX140,56mm. Paroi droite de référenceX144mm. Réserver **X140..170/Y79..105/Z11..30** pour fils, ferrules et entretien. Les vis nécessitent aussi un passage d’outil par le dessus **X132..142/Y80..104/Z12..45** : un boîtier fermé doit laisser ces accès dédiés, éventuellement bouchonnés, pas seulement un trou face au fil.

Ancrages de serre-câble sur la **structure du boîtier**, proposésX154/Y82 et101. Le diamètre des câbles et le rayon de leur boucle de mou restent à définir. Aucun tirage du câble ne doit charger les headers du Click. La paire de borniers, ses fonctionsA/B/GND2 et la révision physique doivent être confrontées au schéma v102 : l’archive mécanique disponible est v101, aucune égalité v101/v102 n’est démontrée par leurs noms.

Cuivre du shield limité àX110mm ; projection du centre ADM2867E v101 versX118,46mm. Ne pas interpréter cette distance géométrique comme une tenue diélectrique certifiée. **A/B/GND2 restent uniquement sur le Click**. Pas de fixation métallique, blindage ou paroi conductrice créant un pont entre domaine isolé et GND logique ; pièces et fixations isolantes prévues. Le serrage et l’isolation de l’assemblage réel restent bloqués avant revue des dimensions.

## Nappe capteur

Réserver un couloir nord **X46..61/Y-14..21mm**, sortie indépendante de l’USB ST-LINK. La liaison du couloir au connecteur réel du MKIGIBV2 est **non figée**, faute d’enveloppe exacte. Selle plate sur le boîtier vers(53,5;5), indépendante du connecteur ; entrée arrondie et absence d’arête coupante. Largeur de nappe nominale7,62mm issue de12conducteurs au pas0,635 ; gorge de selle proposée8,4×1mm, épaisseur réelle à confirmer. Rayon de chemin proposé10mm : **pas une limite constructeur qualifiée** ; rayon minimal admissible non obtenu. Ne pas contraindre la nappe pour faire rentrer le boîtier.

Conserver la nappe ST et ses contacts au pas1,27mm. La déconnexion de maintenance exige d’abord la libération de la selle, puis du raccord, sans traction sur les fils ou les broches. Ni les30cm existants ni SPI3à8MHz ne sont qualifiés par ce plan.

## LED, USER, USB et alimentation

La reconstruction UM2861 situe une baie basse non couverte par le shield vers **X22..92/Y117..131**. Réserver une fenêtre transparente commune ou trois guides optiques courts pourLD1/LD2/LD3 et un poussoir USER **captif purement mécanique**, séparé du RESET qui doit rester en retrait. Aucun GPIO, LED externe ou firmware ajouté. Le modèle `user_plunger.stl` est un échantillon de géométrieØ3/Ø8/longueur14mm ; **sa longueur et sa course ne sont pas adaptées/validées sur C05**. Les centres LED/USER/RESET sont explicitement `null` dans le JSON ; ils ne sont pas inventés à partir d’une photo.

Les zones USB sont réservées fonctionnellement : ST-LINK au nord pour maintenance, **aucune obligation de le laisser connecté en acquisition** ; CN15 au sud pour le câble5V externe. Les rectangles du plan ne sont pas les empreintes exactes des connecteurs. Il faudra confronter les connecteurs, les coques de fiches, le débattement des câbles, les LED et le poussoir à C05 avant de coter les ouvertures. Les jumpers restent accessibles hors tension par maintenance/dépose contrôlée du shield, sans tirer sur les câbles ; accès direct boîtier fermé non promis.

**Avenant V1.1-A du03/11/2026 (main77d67460)** : alimentation de campagne retenue = rallonge secteur230V restant à distance de l’électronique → adaptateur secteur externe5V existant → CN15. **Aucun230V sur le shield ni dans le boîtier TR2** ; seul le câble USB5V y entre. Source et installation provisoire suivant la sécurité électrique du bord, sans conception d’une alimentation secteur dans cette mission. Powerbank et interrupteur USB supplémentaires ne sont plus des exigences et ne conditionnent pas la CAO. Leur étude, comme l’alimentation autonome du produit final, est reportée. La coupure ne remplace jamais STOP/fin propre du writer ; conserver JP6 et la procédure d’alimentation Nucleo de l’essai retenu.

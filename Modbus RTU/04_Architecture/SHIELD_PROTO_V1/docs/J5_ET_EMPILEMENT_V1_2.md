> Historique V1.2 conservé : pour les deux Nucleo réelles, J5_ET_EMPILEMENT_V1_3.md/PRECOMMANDE_V1_3.md remplacent le montage J5. Ne pas modifier les Nucleo.

# J5 et empilage — définition nominale V1.2

**Solution définie, montage réel NON VÉRIFIÉ.** UM2861Rev4 table19 p37 confirme PG4=CN12-69 et PG6=CN12-70 ; UM2861Rev10 §8.2, cité par le contrat/les décisions, décrit Morpho non équipé d'origine. Le PDF Rev10 n'a pas été retéléchargé pendant cette mission ; le Rev4 local a été réellement inspecté. Aucun Morpho femelle supposé présent.

| Pièce | Implantation / référence proposée |
|---|---|
| J5 mâle shield | Harwin M20-9990245, deux contacts pas2,54, carré0,64, pin d'accouplement6,10, isolant2,54, queues3,00mm, **face dessous** |
| Contrepartie nouvelle | **Une M20-7820242 par Nucleo**, femelle simple rangée deux contacts, soudée **en travers des deux rangées** sur la dernière paire CN12-69/70, pas2,54 ; pas sur69/67 dans la même colonne |
| Repère local1 | X85,96/Y111,36mm = CN12-69/PG4→RTU_DE |
| Repère local2 | X88,50/Y111,36mm = CN12-70/PG6→NC côté shield ; soudure mécanique seulement, aucune liaison PG4↔PG6 |
| Hauteur femelle | 8,50±0,15mm corps, queues3,00±0,25mm ; dessin Harwin M20-782 rev11 |
| Engagement nominal | 6,10mm introduits à butée corps contre corps, point de contact femelle à3,41±0,25 sous l'entrée ; marge nominale6,10−3,41=2,69mm après point de contact (pas spécification de tolérance garantie de l'ensemble) |
| Écart plans de cartes | 8,50+2,54=**11,04mm** entre dessus Nucleo et dessous shield ; ajouter1,60 pour le dessus shield. Ce calcul ne prouve pas que les Zio C05 existants ont même hauteur |

J1 M20-9981045 (2×10), J2 M20-9980845 (2×8), J3 M20-9981545 (2×15), J4 **M20-9982045 recoupé à2×17** selon méthode/outillage Harwin. Ne pas commander M20-9981745 : la liste constructeur n'identifie pas cette variante comme active. Tous ont la même famille de pin6,1 et isolant2,54. Les mâles vont dessous ; **ne pas inverser leur numérotation en recopiant la vue miroir**. Le gabarit pages1/2 est la vue dessus d'empilage ; page3 seule est la vue dessous miroir.

L'embase nouvelle de5,48×2,50mm nominal doit tenir entre les pads voisins CN12-67/68, au bord inférieur ; le calcul de longueur donne un dépassement axial1,47mm par rapport à chaque centre extrême. Le pas2,54 donne nominalement1,29mm entre son flanc et les centres de la paire précédente, **sans prouver le dégagement C05 réel**. Aucune modification de la Nucleo n'a été effectuée. Vérifier isolation et accessibilité du fer sous la carte, diamètre réel et métallisation des trous, aucun composant ou tête de vis, puis continuité PG4 uniquement et absence de pont vers PG6/pads voisins lors d'un futur assemblage autorisé. Les trous du shield sont1,02mm ; les trous ST restent inconnus ici.

Entretoises M2,5 isolantes : la dimension fonctionnelle est la hauteur engagée réelle, pas un11mm imposé par serrage. Candidat11mm + rondelles isolantes calibrées selon mesure ; aucun serrage ne doit combler par déformation les0,04mm nominaux, ni compenser un socket Zio d'une autre hauteur. Têtes et rondelles NucleoØ≤4,5mm et contrôles sous carte. Trois ancrages : (32,62;21,19), (33,89;97,39), (79,61;97,39), trous2,8. Le quatrième près de CN7 demeure omis.

Sources primaires : [Harwin mâle J5](https://www.harwin.com/products/M20-9990245), [Harwin femelle](https://www.harwin.com/products/M20-7820242), [dessin M20-782](https://content.harwin.com/asset/53851ba8-b32e-4ac5-a0c5-09d9271c0c0a/DRG-00385-Technical-Drawing-Datasheet-M20-782-pdf.pdf), [famille double](https://www.harwin.com/products/M20-9982045). Les dessins sont consultés localement avec empreintes SHA dans sources-v1-2.json ; les fichiers fabricants ne sont pas redistribués sans licence explicite.

Correction de revue : aux deux ancrages inférieurs, l'écart nominal au bout du corps Zio est seulement2,54mm. Une têteØ5 laisse0,04mm avant tolérances ; elle n'est donc **pas retenue** pour ces ancrages. Limiter tête **et rondelle** àØ4,5mm (rayon2,25), soit0,29mm nominal au corps et0,04mm au courtyard déjà majoré. Candidat réel vis nylon M2,5×6 DIN84, par exemple Accu SFE-M2-5-6-N / Westfield équivalent ; longueur à adapter au filetage de l'entretoise. Ne pas confondre avec vis de module, pour lesquelles le corridor SD admet nominalementØ5. Le contrôle sur C05 reste nécessaire, notamment du côté Nucleo. Source fabricant vis : https://www.westfieldfasteners.co.uk/Bolts-Screws-Metric/Plastic-Machine-Screw-Slotted-Cheese-M2.5x6-Nylon.html

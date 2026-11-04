# V1.4 — CN12 en quinconce, femelle 2×3 autorisée

ST UM2861 Rev8, table19 p37 et vue dessus figure4 p10, a été réellement téléchargé depuis STJapon et inspecté. Les dernières paires sont65/66,67/68,69/70. L'utilisateur confirme ensuite deux barrettes décalées : **67/69 et66/68**. Elles ne forment pas un rectangle2×2. Une vue miroir ne résout pas ce décalage.

L'utilisateur a expressément autorisé **la femelle2×3 et le déplacement d'ancrage**, puis la poursuite autonome ; décision conservée dans l'archive de mission. Le format2×2 du titre initial est donc remplacé, sans renuméroter artificiellement les broches ni modifier les Nucleo.

| Pin local J5 | CN12 / fonction ST | Présence mâle opérateur | Connexion shield | Coordonnées candidates X/Y mm |
|---|---|---|---|---|
|1|65 / PD10|absent|NC indépendant|85,96 /106,28|
|2|66 / PG8|présent|NC indépendant|88,50 /106,28|
|3|67 / PG7|présent|NC indépendant|85,96 /108,82|
|4|68 / PG5|présent|NC indépendant|88,50 /108,82|
|5|69 / PG4|présent|**RTU_DE**|85,96 /111,36|
|6|70 / PG6|absent|NC indépendant|88,50 /111,36|

Les coordonnées reprennent le repère du PCB antérieur reconstruit sur UM2861 ; elles ne sont pas une extraction de CAO C05. Les deux alvéoles65/70 vides n'imposent aucun mâle supplémentaire sur Nucleo. Les cinq autres pads sont des îlots NC séparés, pas un bus reliant GPIO. PG7/PG8 conservent leurs liaisons LPUART par Zio ; aucun second signal n'est câblé par J5.

## Composant sélectionné et pose

**Samtec SSW-103-01-F-D**, standard femelle traversante2×3, pas2,54 dans les deux axes. Catalogue : corps8,51 de haut,4,95 de large, longueur3×2,54+0,51=8,13mm ; queuesstyle01=2,64mm ; entrée pour mâle nominal.025pouce carré(~0,635mm), profondeur d'insertion **3,68 à6,35mm**. Dessin d'implantation double Samtec : trous1,04mm. Les six perçages J5 sont donc1,04 ; autres connecteurs1,02 conservés. Aucun appui sur une compatibilité de section réelle non observée.

Corps sous le shield, queues vers le dessus. Empreinte active `J5_CN12_65_70_2X3_FEMALE`, B.Fab/B.CrtYd et légende miroir «5=69PG4». Ne pas inverser les coordonnées en recopiant une vue dessous : le gabarit dessus montre la numérotation ST, la page miroir sert seulement au contrôle. La femelle passive n'a aucun pont interne entre contacts ; contrôler les soudures et la continuité DE hors tension à l'assemblage autorisé.

La Connfly2×3 économique est documentée comme alternative non adoptée : elle aurait le même pas, mais sa profondeur de contact n'a pas été trouvée. **Ne pas lui attribuer la plage Samtec**. Le choix Samtec documente cette exigence pour deux pièces, sans remplacer les autres barrettes économiques.

## Empilage avec les mesures déjà acquises

Pointe mâle depuis PCB :8,22mm ; sommet du plastique :2,40 ; métal exposé :5,82. Mesures approximatives utilisateur, incertitude non indiquée ; ne pas les redemander. Avec G=écart dessusNucleo→dessousshield et Hf=8,51 nominal :

- profondeur I=8,22−G+8,51 ;
- jeu entre corps C=G−8,51−2,40 ;
- à G11,00 **hypothétique**, I5,73 et C0,09mm ;
- en combinant3,68≤I≤6,35 et C≥0 : **10,91≤G≤13,05mm**, intervalle nominal sans réserve de tolérance.

La hauteur naturelle des Zio et les tolérances du composant/mesures ne sont pas connues. Les0,09mm ne garantissent donc aucun jeu positif réel. Aucun serrage/flexion ni engagement partiel imposé pour masquer un conflit. Ne pas transformer ce calcul en qualification matérielle.

## Fixation Click et dégagements

La proposition2×3 initiale croise le trouM3 de l'étrierClick(91;108) : DRC initial conservé. Après accord utilisateur, déplacement vers **(94,5;112,5)**, autres ancrages conservés ; étrierSCAD/STL adapté, retenue vissée maintenue. Les trajectoires des32routes sont identiques ; seule l'étiquette de terminaison J5.1→J5.5 change pour DE, resté à ses coordonnées.

Têtes/rondelles NucleoØ≤4,5 hors facesPCB et entretoises candidatesØ≤4 dans l'espace entrecartes, comme V1.3. Dégagement nominal très faible prèsZio : fit réel encore nécessaire. Nouveau STLClick compilé une fois, fermé/connexe ; pas une preuve de résistance thermique/vibratoire ou de contact sur modules.

Sources : [STUM2861Rev8](https://www.stmcu.jp/wp/wp-content/uploads/2021/10/UM2861_Rev8.pdf), [SamtecSSW](https://suddendocs.samtec.com/catalog_english/ssw_th.pdf), [perçagesSamtec](https://suddendocs.samtec.com/prints/ssw-d.pdf), [référence sélectionnée](https://www.samtec.com/products/ssw-103-01-f-d). URLs, SHA et observations dans sources-v1-4.json.

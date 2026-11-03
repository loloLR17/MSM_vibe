# Assemblage futur V1.3 — aucune manipulation dans cette mission

Lire PRECOMMANDE_V1_3.md et J5_ET_EMPILEMENT_V1_3.md. **NE PAS COMMANDER.** Les deuxNucleo gardent leurs mâlesCN12-69/70 ; ne rien dessouder ni ajouter dessus.

1. Après autorisationd'assemblage et contrôle C05/hauteurs, banc hors tension : souder J1–4 **mâles dessous** puisJ5 **femelle dessous**, queues dirigées vers le dessusshield, tous corps selon leur hauteur propre. Les cinq connecteurs ne sont pas du même genre, ne pas souderJ5 comme un cinquième mâle. Numérotation vue dessus1→69PG4DE,2→70PG6NC.
2. Contrôler visuellement soudures/décalages et continuité DE sans pontPG6/voisins. Emboîter sans force ni flexion, comparer l'engagement naturel J1–5 ; entretoises à hauteur réelle, OD≤4mm candidat dans l'espaceentre cartes, têtes/rondelles≤4,5mm horsfacesPCB. Pas de serrage pour tirer un contact partiellement engagé.
3. Poser sockets femelles J6–J9 **dessus**, à longueurs exactes. Vérifier queues propres/absence de court. FRAMJ6-9VIN3V3/J6-8sortieLDO NC, J7-1=3V3/2=GND. Garder découplage, SPI1/2/3 et PG4DE, aucun5V injecté.
4. Poser fixation Nucleo cachée avantSD. FRAM etSD chacun deux entretoises/vis isolantes existantes ; aucune fixation ne sert de butée électrique, aucun appui sur composants. SD sort-X, pas de sangle devantfente.
5. MKIGIBV2 JP1gauche1/2en haut(VDDIO/VDD), JP2droite24en haut(MISO)/13en basGND. Entraxe15,24nominal issu du15,2utilisateur ; nappe-Y avec reprise d'effort. Click côtéisolé/borniers+X, VCCSEL3V3 et pontDE-/RE conservés, aucun pontGND2/GNDlogique ou visconductrice traversantbarrière.
6. Fixer étriersJ8/J9 aux ancrages conservés, patins ajustables sur rives libres, pas surcontacts/composants. Tous modules restent démontables. Détrompageautomatique non requis : repèresbroche1 et contrôle manuel hors tension obligatoires.
7. Adapter ultérieurement selles/boîtier, accès CN15/ST-LINK/LED/USER, RESET protégé. Aucun230V surshield. Les modèlesSTL restent REVIEW_ONLY inchangés deV1.2, aucune impression ni tenuevibratoire qualifiée ici.

Dernier relevé nécessaire : gabaritdeuxC05 + cotesmâlesJ5/engagementZio + enveloppesMKI/Click, regroupés ultérieurement. Pas de mesureshistoriquesMKI redemandées. Une qualification électrique/thermique/vibratoire surprototyperéel exigera sa mission et ses autorisations propres.

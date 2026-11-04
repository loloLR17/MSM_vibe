# V1.4 — candidat 2×3, fabrication non libérée

**NE PAS COMMANDER.** L'identifiant de mission conserve2X2, mais le format2×3 et le déplacement de fixation sont autorisés explicitement par l'utilisateur durant l'exécution. La confirmation du quinconce66/67/68/69 explique la contradiction initiale ; aucune broche Nucleo n'est déplacée ou ajoutée.

| État | Sens courant |
|---|---|
| CAD_VERIFIED | ERC/DRC stricts, parité et comparaison électrique/CAM du candidat passent |
| READY_FOR_PHYSICAL_TEMPLATE_CHECK | GabaritV1_4 A4, vues dessus/de dessous et calcul Z prêts |
| BLOCKED_FABRICATION | Hauteur naturelle Zio, fit sur les deux C05, dégagements/modules et tolérances restent à démontrer |

Lire PINOUT_ET_Z_V1_4.md, BOM/budget et ASSEMBLAGE_ET_RESERVES.md. Ouvrir le projet historique TR2_Shield_Proto_V1.kicad_pro, cartouche1.4. J5 SamtecSSW-103-01-F-D6contacts, local5=PG4, autresNC ; 191pads dont11NPTH,59connexions et4vias, contour90×100. Quatre trous supplémentaires65..68 et diamètreJ51,04 ; DE à la même position. L'ancrage inférieurClick passe de91/108 à94,5/112,5, étrier adapté ; aucun reroutage géométrique, autre module/ancrage Nucleo inchangé.

STUM2861Rev8 reçu et vérifié pour les noms et paires. Les ZIP officielsMB1549 restent inaccessibles après deux tentatives bornées8,24s ; aucun contenu/révision/hash connu, aucune assimilation C02/C03→C05. La géométrie reste celle du candidat, pas un ajustementC05 prouvé.

**Unique contrôle physique groupé restant**, à prévoir ultérieurement sous autorisation, sans nouveau relevé8,22/2,40 ni entraxe15,2 : confronter le gabarit100% sur les deuxC05 et constater l'emboîtement naturel J1–J5 (G), sans force, avec dégagement des corps/queues/entretoises et des réservationsMKI/Click. Une vue de profil de l'ensemble engagé, accompagnée de G, est utile pour Z ; une photo dessus seule ne le démontre pas. L'absence de collision sur les volumes modules est également à confronter aux cartes réelles avant libération.

Les guidesLED, boîtier complet, poussoir et patins finaux d'étrier peuvent être ajustés après PCB si leurs réservations sont respectées. Le détrompage automatique abandonné ne bloque pas les PCB nus. Ni achat ni validation physique ne sont impliqués dans ces exports candidats.

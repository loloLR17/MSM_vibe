# Assemblage futur V1.4 — quinconce reçu par une femelle2×3

Aucune manipulation physique dans la mission actuelle. **NE PAS COMMANDER**, vérifier d'abord PRECOMMANDE_V1_4.md et PINOUT_ET_Z_V1_4.md.

1. Les Nucleo gardent leurs deux paires mâles décalées67/69 et66/68. Ne pas ajouter65/70 ni dessouder de contacts. J5femelle2×3 se soude **sous** shield, queues dessus ; J1–J4 restent mâles dessous, J6–J9 femelles dessus.
2. Repère local1=CN12-65, local5=CN12-69/PG4/DE ; local1/2/3/4/6NC. Entrées65/70 vides côté Nucleo. Contrôle croisé vue dessus/miroir et soudures sans pont ; aucune broche décalée ne doit être forcée dans une2×2.
3. Après autorisation d'assemblage, emboîter les cinq connecteurs naturellement et mesurer G une fois. Les mesures mâles8,22/2,40 sont acquises. Ajuster les entretoises à la hauteur compatible, jamais tirer les contacts par serrage. Vérifier section/entrée réelle et dégagement sous carte. Têtes/rondellesØ≤4,5 horsPCB ; entretoises entrecartes candidatesØ≤4.
4. Garder les trois ancragesNucleo et poser celui caché sousSD avant modules. FRAM/SD restent vissées chacune sur leurs deux entretoises. J5 queues2,64 nominal dépasse d'environ1,04 au-dessus d'unPCB1,6 ; tolérances et composants réels à contrôler.
5. L'ancrage inférieurClick autorisé est maintenant94,5/112,5, perçage3,2 ; supérieur91/75 conservé. Utiliser l'étrierV1.4, pas le STL antérieur. Patins sur rives libres, aucun appui sur contacts/composants. Nouveau modèle REVIEW_ONLY non qualifié thermiquement/mécaniquement et aucune impression commandée.
6. FRAM VIN3V3 et sortieLDO NC ; microSD fente-X, Click/borniers+X, nappe-Y avec reprises d'effort. Garder GND2/A/B isolés du GNDlogique. Aucun5V nouveau ni230V dans le shield. Détrompeurs automatiques non requis, repérage manuel hors tension obligatoire.
7. Préserver CN15, ST-LINK, USER/LED et RESET protégé ; boîtier/poussoir/guides finis différés. Toute qualification électrique/thermique/vibratoire exige le prototype réel et sa mission propre.

> Historique V1.3 : les mâles enquinconce66/67/68/69 et l’accord2×3 invalident le raccord1×2 ; PRECOMMANDE_V1_4.md/PINOUT_ET_Z_V1_4.md prévalent.

# Connecteurs standard économiques — sélection V1.3

Les dessins **constructeurs** Connfly legacyDS1021/DS1023 et Neltron2212S ont été réellement téléchargés et inspectés, y compris les images des pages non textuelles. Sources/hashes dans sources-v1-3.json. Les prix/stock proviennent des distributeurs, ne sont ni un devis ni une preuve de stock physique.

| Usage | Référence / dimensions utiles | Décision |
|---|---|---|
| J1–4 dessous | Connfly DS1021-2*40SF11, double2,54, carré0,64, corps2,5, tail3,0/mating6,1 (legacytype1),−40..105°C | Trois barrettes2×40 pour les deux shields ; recoupe contrôlée, ébavurage, contrôle visuel d'absence de fissure/pin mobile. Pas de conducteur relié entre contacts |
| J5 dessous | Connfly DS1023-1*2S21 H8,5, goldflash,−40..105°C, contactalliageCu, trou1,02 | Exact1×2 ; dimensions mâles existantes et profondeur de contact restent à vérifier, pas de pièce sur Nucleo |
| J8 dessus |2×DS1023-1*12S21 par shield, H8,5 | Exact12, **pas de découpe femelle supposée sûre** |
| J9 dessus |2×DS1023-1*8S21 par shield, H8,5 | Exact8 ; isolationClick et étrier conservés |
| J6/J7 dessus | Neltron2212S-09G-85, H8,50,tail3,10±0,3 ; A23,36±0,25,largeur2,5 | Alternative9 disponible affichée chezTayda ; DS1023-1*9S21 actuellement0stockTME, donc non présentée disponible |

Toutes les interfaces ont un pas2,54 et des queues traversantes avec trou1,02 nominal dans les dessins ; perçages PCB inchangés V1.2, fabricantPCB devra garantir le fini demandé. Les modules existants doivent conserver des mâles acceptés par ces sockets : inspection de leur section/longueur et absence d'usure à l'assemblage. Pas de fiche validant implicitement une nappe30cm/8MHz, tenue maritime ou endurancevibratoire. Le goldflash de barrettes standards ne remplace pas la retenue positive ; les vis, entretoises, étriers et reprises d'effort restent prévus.

**Piège de référence écarté :** `DS1021-01` actuel H4,3mm n'est pas `DS1021` legacy H2,5 ; ne pas mélanger leurs tables de longueur. PourDS1023, le catalogue indique3,2mm de queue et le dessin3,0 : 3,2 est un nominalcatalogue, **pas un maximum garanti** ; longueur livrée et tolérance encore à vérifier avant libération. Les tolérances Harwin ne sont pas réutilisées pour Connfly.

Plan de débit prudent des trois barrettes mâles40paires :

- Barre1 :17 +1 sacrifiée +17 =35 ; reste5.
- Barre2 :15 +1 +15 +1 +8 =40.
- Barre3 :10 +1 +10 +1 +8 =30 ; reste10.

Total100paires nécessaires (J1=2×10,J2=2×8,J3=2×15,J4=2×17), cinq sacrifiées et15 restantes. C'est un **plan d'atelier candidat**, pas une découpe déjà réalisée ni une garantie du fabricant sur l'outil choisi ; en cas de corps fissuré, remplacer la section, ne pas la retenir par colle ou friction. Sockets femelles achetées à longueur exacte. Le retour à Harwin reste une alternative documentaire plus coûteuse, pas une substitution silencieuse de hauteur/empreinte.

Quantités réelles à acheter si l'utilisateur décide ultérieurement : male3, femelle2×10, femelle12×10, femelle8×10 (MOQ10), femelle9×4. Coût connecteurs5,66USD, incluant surplus. La présence d'un deuxième fournisseur de composants peut annuler une partie de l'économie via le port ; voir BUDGET.md. Aucune commande effectuée.

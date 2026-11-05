# J5 et passifs — comparaison ciblée, sans modification KiCad

Les identifiants de sources ci-dessous renvoient à [sources.json](sources.json). Les observations du PCB proviennent du fichier natif et du readback V1.9 conservés ; les preuves numériques V2.0 et V2.1 sont réutilisées, sans nouvelles validations physiques.

## J5 : la profondeur de contact décide

| Caractéristique | Samtec SSW-103-01-F-D conservé | Connfly DS1023-2*3S21 proposé |
|---|---:|---:|
| Configuration | Femelle 2×3, pas 2,54 mm | Femelle 2×3, pas 2,54 mm |
| Corps | H8,51 ; largeur4,95 REF ; longueur8,13 nominale | H8,5 ; largeur5 ; longueur8,12 nominale |
| Queue | 2,64 mm | Dessin3,0±0,2 ; page TME3,2 |
| Profondeur d'insertion documentée | **3,68–6,35 mm**, catalogue Samtec | **Non trouvée** dans le dessin Connfly consulté |
| Stock/prix affiché | DigiKey107, MOQ1, **1,15 € HT** | TME12910, lot10 à0,10 USD HT/unité |
| Conclusion | Référence retenue, emboîtement réel encore à confronter | **NO-GO substitution** |

Sources : [catalogue Samtec](https://suddendocs.samtec.com/catalog_english/ssw_th.pdf), [series print exact V2.0](https://suddendocs.samtec.com/prints/ssw-1xx-xx-xxx-x-xx-xxx-xx-mkt.pdf), [dessin Connfly double rang](https://www.tme.eu/Document/091d1fadf99a8576d78359e8cef92fb6/DS1023.pdf), pages D1/T5. Le téléchargement HTTP local du dessin Connfly a répondu403 ; il a été lu par l'outil Web, sans contournement ni empreinte de fichier inventée.

Le shield porte six trous Ø1,04 avec pas nominal2,54 ; Connfly recommande Ø1,02 et donne un pas2,54±0,1. La proximité des cotes suffit à identifier une piste nominale, pas à approuver l'assemblage. Un corps presque identique ne garantit ni début de contact, ni profondeur avant talonnage, ni effort de retenue.

Implantation inchangée **sous shield**, queues de soudure vers le dessus. Les contacts locaux1..6 correspondent à CN12-65..70 ; les mâles réellement rapportés en66/67/68/69 entrent dans les contacts2/3/4/5. **Seul local5 = CN12-69 = PG4 est relié à RTU_DE ; les cinq autres sont NC.** Aucune liaison ou court-circuit supplémentaire n'est créé pour les trois autres mâles présents ou pour les deux positions vides.

À G=12,25±0,10, le budget V2.0 donne pour Samtec **I5=4,15–4,81 mm**, garde plastique minimale1,01 ; les Zio donnent Iz=4,00–5,72 et garde0,63, sous hypothèses documentées. Pour Connfly, le calcul purement géométrique nominal serait8,22+8,50−12,25=4,47 mm. **On ne peut pas comparer ce résultat à une zone de contact inconnue.** Aucune plage d'insertion Samtec n'est prêtée à Connfly. Pas de remplacement, pas de nouvel export PCB.

## J6/J7 et conditionnements

La [femelle Connfly1×9 T4](https://www.tme.eu/en/details/zl262-9sg/pin-headers/connfly/ds1023-1-9s21/) a neuf contacts, H8,5, corps nominal23,36×2,5, pas2,54. Elle reste dans l'enveloppe nominale1×9 réservée au module ; tails dessin3,0±0,2/pageTME3,2 à confronter et recouper si nécessaire. **Stock affiché zéro et aucune date de réapprovisionnement établie** : prix0,12USD/lot10 non assimilé à disponibilité.

Le [Harwin M20-7820946](https://www.harwin.com/products/M20-7820946), proposé en V1.8, donne23,26×2,5/H8,5, queue3, pin d'accouplement carré0,64 et ne se coupe pas. DigiKey affiche4344 et1,18€HT à l'unité (D2). Le contact étain diffère de la référence Neltron dorée ; compatibilité et retenue des mâles de modules restent à observer. Aucun nouveau modèle imposé dans KiCad.

Pour deux shields : quatre1×9 et non deux ; quatre1×12 et quatre1×8 car chaque module a deux rangées. TME impose10 pièces pour les familles proposées. Les trois barres mâles2×40 existantes fournissent120 colonnes, dont100 nécessaires à deux jeux10/8/15/17 ; elles sont prévues coupables entre colonnes. Cela n'autorise pas à raccourcir arbitrairement une femelle moulée1×12 pour obtenir1×9.

## C1 : AVX possible, avantage financier limité

[SR205C104KAR](https://www.tme.eu/en/details/sr205c104kar/mlcc-tht-capacitors/kyocera-avx/) et son [dessin fabricant SR20](https://www.tme.eu/Document/8bd74184c9c9f1c71ca9a7c545331f20/skycap-sr.pdf) :100nF,50V,X7R,±10%, pas2,54 ; longueur/hauteur maximales5,08 et épaisseur3,175, fil nominalØ0,508. Le suffixeR désigne des queues longues≥25,4mm : **à recouper**, jamais laissées telles quelles dans l'empilage. Stock affiché3006 ;3×0,67USDHT=2,01USD contre10×0,36=3,60 : le palier10 n'est pas économique pour ce besoin2+1.

Lecture native C1 : pads(38;52) et(40,54;52), perçagesØ0,8, dessus du shield. Le corps centré entre les fils donne Xrel−1,27..3,81/Yrel±1,5875. Il dépasse de0,01mm la représentationFab droite existante(3,8) et de0,1875mm le courtyardY(±1,4). **Cet écart est signalé, pas corrigé silencieusement.** La Fab existante est plus épaisse enY(±2,5) ; aucun contact voisin n'est identifié dans l'extension. L'entraxe nominal et la marge de trou sont cohérents, mais cela ne constitue ni DRC de la variante intégrée ni qualification d'assemblage. Gabarit de têtes/modules et pose réelle à vérifier au premier article ; queue finie≤3,5mm selon budget V2.0.

Le Kemet actif [C315C104K5R5TA](https://www.digikey.fr/fr/products/detail/kemet/C315C104K5R5TA/12701330) est affiché3937 en stock et0,70€HT. Pour trois pièces, conserver cette référence augmente le budget livré d'environ0,37€ seulement. Ce repli évite d'adopter AVX pour un gain minime ; aucun changement de BOM électrique n'est effectué.

## Autres passifs et accessoires

Vishay10kΩ1% : dix pièces au palier0,16USD coûtent1,60USD, moins que huit au palier0,35(2,80USD). Huit montées, deux secours. C2 `EEUFR1H100`, variante V1.8 conditionnelle de `ECA1HM100`, reste10µF50V/Ø5×11/P2,0 ; stock29481,3×0,32USD. Respecter la polarité ; pas d'adoption silencieuse de ce changement.

Pas d'ajout de détrompeurs, rondelles métalliques ou prises supplémentaires. Les queues et soudures ne doivent pas dépasser les enveloppes d'acceptation ; le pinout, le côté de montage, les NC et l'isolement RS485 restent ceux de V1.9. Résultats lisibles par machine dans [compatibilite-candidats.json](preuves/compatibilite-candidats.json).

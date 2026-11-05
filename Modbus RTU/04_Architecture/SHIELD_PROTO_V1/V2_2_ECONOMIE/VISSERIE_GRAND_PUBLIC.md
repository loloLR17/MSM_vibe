# Visserie nylon grand public — offres exactes et réserves

**Ajile est une piste identifiable et sensiblement moins chère, mais sa vis VCY120 n'a pas encore une tête cotée démontrée compatible.** L'écrou est coté. Les anciens prix de recherche ou le prix par défaut d'une famille ne sont pas utilisés à la place des variantes exactes.

## Variantes vérifiées en lecture seule

| Pièce | Offre directe sélectionnée | Prix TTC / quantité d'étude | Preuve dimensionnelle |
|---|---|---:|---|
| Vis M2×20 PA6.6 | [Ajile VCY120](https://ajile.biz/fr/vis-plastique/486-vis-plastique-tete-ronde-fendue.html?group%5B11%5D=423), produit486/attribut2190, EAN3701015116850 | 0,30 € ×8 =2,40 € | [Image de variante VCY120](https://ajile.biz/12765-large_default/vis-plastique-tete-ronde-fendue.jpg) et [dessin associé](https://ajile.biz/12767-large_default/vis-plastique-tete-ronde-fendue.jpg) : M2/longueur20. **Diamètre et hauteur de tête non cotés.** |
| Écrou M2 PA6.6 | [Ajile UHX001](https://ajile.biz/fr/rondelles-ecrous-plastique/424-ecrou-plastique-hexagonal-pour-vis-m2-a-m12-naturel.html?group%5B17%5D=376), produit424/attribut1991, EAN3701015107483 | 0,42 € ×16 =6,72 € | [Image cotée associée](https://ajile.biz/12448-large_default/ecrou-plastique-hexagonal-pour-vis-m2-a-m12-naturel.jpg) : AF4mm et H1,6mm ; coins calculés4,619mm, tolérances non données. |

Méthode : GET de page avec attribut de variante, lecture de `data-product.attributes`, prix et MOQ ; aucune mise au panier. Les références de famille sont VCY000/UHX000, les références **des variantes** VCY120/UHX001. Sans sélection, les pages affichent notamment M2×5 ou M12 : ces prix ne qualifient pas notre lot. Les prix exacts sélectionnés et empreintes des JSON locaux sont conservés dans [preuves/ajile-variantes-extraites.json](preuves/ajile-variantes-extraites.json) ; HTML/JSON/images complets restent hors Git sous `/home/lolo/dev/msm/reports/TR2-shield-v22-20261105/`.

Les deux variantes affichent **MOQ1**, disponibles. L'ancien minimum10 de la vis, cité comme piste dans la mission, n'est pas celui de la page actuelle sélectionnée. Le champ999999 n'est pas un comptage audité du stock : aucune quantité physique précise n'est affirmée. Matière PA6.6 selon vendeur, pas certificat de lot.

## Compatibilité mécanique et limite localisée

Besoin6 vis +12 écrous, réserve2+4 : **8/16**. Enveloppe V2.1 : vis M2×20 sous tête, têteØ3,8/H1,3 sous Nucleo ; écrous AF4/H≤1,6, diamètre aux coins4,619, deux superposés au-dessus du shield. Pas de vis métal, rondelle large, tête fraisée ou M3 de substitution.

L'écrou Ajile correspond aux dimensions **nominales** du modèle V2.1. La longueur et la matière de la vis sont documentées ; les images ne donnent pas Øtête/Htête, et une règle proportionnelle sur une photo ne serait pas une cote. **NO-GO pour déclarer cette vis interchangeable sans réserve.** Avant utilisation, il faut une cote fournisseur ou le contrôle à froid du lot réel satisfaisant l'enveloppe ; une tête plus grande exige une vérification dédiée, jamais un forçage ou une reprise Nucleo.

Avec les dimensions V2.1, la pile nominale sous tête vaut18,65mm, laissant1,35mm de filet sur20. Sous les budgets V2.1 L≥19,8, G≤12,35, PCB≤1,76 et écrous≤1,6, il reste≥0,73mm. Ce calcul ne certifie ni les tolérances non données d'Ajile, ni la tenue des filets, ni un couple de serrage. D126 et le contreblocage restent à confronter au premier article.

## Prix acheminés France et délais

Marchandises7,60€HT = **9,12€TTC**. Le [barème Ajile France](https://ajile.biz/fr/content/1-livraison) pour commande4,51..15€HT prévoit3,90€HT sans suivi ou5,90€HT suivi. Donc, pour notre lot exact :

| Mode | Port TTC | Marchandises + port TTC | Délai publié après préparation |
|---|---:|---:|---|
| Sans suivi | 4,68 € | **13,80 €** | 5–10 jours ouvrés |
| Avec suivi | 7,08 € | **16,20 €** | 3–5 jours ouvrés |

Expédition annoncée sous48h après paiement, pas date de réception garantie ni devis accepté. Le suivi est une option raisonnable pour limiter le risque postal ; il n'est pas présenté comme une compatibilité mécanique. Un regroupement avec du matériel réellement nécessaire ou du stock personnel reste prioritaire, sans achats supplémentaires pour atteindre artificiellement un seuil de port gratuit.

Repli documenté V2.1 : [vis DIN84 PA M2×20 chez SAM/ScrewsAndMore](https://www.screwsandmore.de/25-stueck-zylinderkopfschrauben-din-84-polyamid-m2x20/84pa220-25) et écrous DIN934 PA M2. Enveloppe nominale adaptée, lots25+25, budget France≈25,90€ avec port, **non actualisé et non devis ferme**. Ce repli identifié ne justifie pas de revenir au panier dédié disproportionné rejeté par l'utilisateur. L'étude Ajile ne masque pas la cote de tête manquante pour annoncer une alternative intégralement validée.

## Amazon : pas de kit qualifié

La piste [ASIN B0B18CFB4D](https://www.amazon.fr/dp/B0B18CFB4D) n'a pas été accessible en page directe par l'outil Web. Les listings de kits mélangent longueurs et matières ; aucune offre vérifiée réunissant **nylon isolant + M2×20 exact + dimensions de tête/écrou + prix/port France/stock** n'a été obtenue. **Aucun kit Amazon déclaré compatible**, aucun prix issu d'un moteur de recherche utilisé comme devis et aucun contournement de l'accès marchand.

La recherche s'arrête ici conformément à la mission courte. Les manques sont localisés : tête/tolérances Ajile et offre Amazon non qualifiée, sans bloquer la publication de l'étude ni remettre en cause les preuves D126 acquises.

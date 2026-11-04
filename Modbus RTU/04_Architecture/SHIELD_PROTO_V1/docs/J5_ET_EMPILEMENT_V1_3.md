> Historique V1.3 : les mâles enquinconce66/67/68/69 et l’accord2×3 invalident le raccord1×2 ; PRECOMMANDE_V1_4.md/PINOUT_ET_Z_V1_4.md prévalent.

# J5 V1.3 — femelle sous shield, mâles Nucleo conservés

**Fait rapporté par l'utilisateur03/11 :** les deux NucleoC05 portent déjà des mâles sur CN12-69/70. Aucune cote ni photo/contrôle physique recueilli par Codex. La recommandation V1.2 de modifier les Nucleo est périmée pour ces deux cartes. Aucun dessoudage, ajout de femelle ou autre intervention sur Nucleo.

| Élément | Définition candidate |
|---|---|
| J5 | Connfly **DS1023-1*2S21**, femelle1×2 traversante sous shield, entrée vers Nucleo, queues vers le dessus du shield |
| Numérotation vue dessus shield |1=(85,96;111,36)→CN12-69/PG4/RTU_DE ;2=(88,50;111,36)→CN12-70/PG6/NC |
| Sens physique | Paire en travers des deux rangées Morpho ; pas2,54. Ne pas la déplacer vers69/67 ni inverser1/2 en regardant dessous |
| Corps | Longueur nominale5,58 =2×2,54+0,50 ; largeur2,54, hauteur8,5 nominal. Courtyard conserve les tolérances du dessin, pas de hauteur Harwin±0,15 réutilisée |
| Perçages |1,02mm shield, même nombre/centres/nets queV1.2 ; empreinte active **J5_CN12_69_70_PG4_FEMALE**, B.Cu/B.CrtYd/B.Fab |
| Queues et soudure | Catalogue TME3,2mm, dessin legacy3,0mm et source actuelle3,0±0,2 : à3,2 nominal il resterait1,6 au-dessus d'un PCB1,6 ; **maximum garanti non établi**, réserver le dégagement selon la queue livrée et sa tolérance. Ne pas présenter ces valeurs comme une mesure des pièces livrées. Souder côté dessus sans pression latérale |
| Broches existantes | Section, hauteur du corps et longueur exposée **inconnues** ; ne pas les déduire de «mâles soudés». Les cotes0,60×0,40 du dessin DS1023 sont celles des queues femelles, pas une plage garantie de pin mâle admissible |

Le footprint et le symbole ont effectivement changé de nom/valeur ; un simple changement de texte dans la BOM ne suffirait pas. La face dessous est conservée sans retourner les coordonnées des pads. Le contour du boîtier et deux entrées sont surB.Fab, avec légende miroir «J5 FEM /1=69 PG4» dessous. Le gabarit pages1/2 est vu du dessus pour superposition, page3 est le miroir de contrôle, **pas à superposer** sur une Nucleo vue dessus.

## Calcul conditionnel de hauteur — pas une validation d'engagement

Noter G = dessusNucleo→dessousshield, Hf = hauteur corps femelleJ5, hm = hauteur corps mâleNucleo, Lm = longueur exposée mâle. Entrée femelle à **G−Hf**, pointe mâle à **hm+Lm** ; profondeur d'insertion **I=hm+Lm−G+Hf**. Éviter collision des corps : G−Hf≥hm. I positif seul ne prouve ni contact fiable ni absence de talonnage ; profondeur de contact/fond admissible et emboîtement réel à contrôler.

Cas illustratif, expressément NON MESURÉ : ZioNucleo8,5 + mâles dessous shield Connfly2,5 →G11,0 ; Hf8,5, hm2,5, Lm6,1 donneraient entrée2,5 etI6,1, corps contre corps. Un mâle existant plus haut ou une autre femelleZio peut empêcher cet engagement simultané. Ne pas forcer, jouer sur les entretoises ou accepter un Zio partiellement engagé pour masquer l'écart. Si les cotes ne conviennent pas, choisir une autre hauteur de femelle vérifiée et corriger l'empreinte/dossier avant libération.

**Dernier relevé J5 réellement utile** lorsque les cartes seront accessibles sous mission autorisée : une vue dessus/de profil avec règle de chaque paire69/70 ; hauteur corps et longueur exposée, section carrée ou autre, écartG à l'emboîtement naturel des Zio. Ne pas redemander les cotesMKI acquises. Aucun geste demandé maintenant, utilisateur en trajet ; la CAO indépendante est finalisée.

## Visserie — revue des volumes en Z

Trois ancrages et trous2,8 conservés. Têtes/rondellesØ≤4,5 **hors faces extérieures des deux PCB**, entretoisesM2,5 **entre cartes**. Une tête sur dessusshield n'est pas coplanaire avec un header sousshield ; vérifier séparément les queues/pads traversants. Le modèle calcule >0,9mm nominal tête→bord de pad voisin pourØ4,5 (tolérances réelles à confirmer).

Le corps Connfly mâle a une longueurN×2,54±0,5 : réserver1,77 en bout avec0,25 de marge. Aux deux ancrages inférieurs, une entretoiseØ4,0 laisse0,04 au courtyard majoré et0,29 au corps à longueur maximale ; **Ø5 ne peut pas être supposé compatible**. La dimension extérieure4,0 est une spécification candidate, référence/longueur et tenue mécanique à vérifier sur assemblage ; aucun serrage pour rattraper une flexion. Cette distinction corrige la confusion possible d'une simple projection2D tête/corps, sans prétendre lever le contrôleC05. Si un composant Nucleo ou sa tolérance enlève le dégagement, la libération reste bloquée.

Sources exactes/dessins/hashes : `sources-v1-3.json`. Le dessin DS1021-01 à corps4,3 n'est pas celui sélectionné ; le DS1021 legacy lié à la référenceTME a un corps2,5, pointes6,1 et queues3,0.

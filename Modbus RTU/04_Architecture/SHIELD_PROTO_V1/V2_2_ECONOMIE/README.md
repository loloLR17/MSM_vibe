# V2.2 — économie TME et clips : décision d'étude

Mission `TR2-20261105-SHIELD-V22-TME-CLIPS`, consultation du 5 novembre 2026. **Étude terminable avec réserves localisées ; aucun achat, aucune libération de fabrication.** Le PCB V1.9 et les candidats mécaniques V2.0/V2.1 restent inchangés.

La voie pragmatique est de conserver **J5 Samtec exact et D126**, puis de regrouper les achats électroniques chez TME et DigiKey si aucun stock personnel exact n'est disponible. Le tout-TME n'est pas démontré réalisable actuellement. Les clips simples imprimés ne sont pas retenus dans cette tranche. L'offre Ajile est moins coûteuse que l'expédition dédiée V2.1, mais la compatibilité complète de sa tête de vis reste à démontrer.

## Décisions techniques

| Élément | Verdict | Motif et suite |
|---|---|---|
| J5 Connfly `DS1023-2*3S21` | **NO-GO substitution** | Profondeur utile/zone de contact absente du dessin. Hauteur 8,5 mm seule insuffisante. Conserver `SSW-103-01-F-D`. |
| J6/J7 Connfly `DS1023-1*9S21` | Compatible nominalement ; **NO-GO délai court** | Stock affiché zéro, réapprovisionnement non daté. Ne pas transformer une femelle 12 contacts en 9 sans preuve fabricant. |
| J6/J7 Harwin `M20-7820946` | Repli V1.8, **GO conditionnel** | Disponible chez DigiKey à l'unité, enveloppe nominale documentée ; engagement des modules et placage étain à confronter au premier montage. |
| C1 AVX `SR205C104KAR` | **GO conditionnel de variante d'étude** | Valeur et pas corrects ; queues à recouper. Débordements nominaux de Fab/courtyard signalés dans la note technique, sans obstacle local identifié ni modification PCB. |
| Double clip FFF simple | **NO-GO adoption dans cette mission** | Jeu axial et précharge, flexion des ergots et accès de démontage non maîtrisés ensemble. Pas de nouveau STL arbitraire. |
| D126 V2.1 | **Candidat conservé** | Preuve géométrique bornée réutilisée ; tenue au fluage/vibrations et fabrication réelle toujours non qualifiées. |
| Vis Ajile VCY120 + écrous UHX001 | Offre exacte identifiée ; **compatibilité partielle** | PA6.6, M2×20, écrou AF4/H1,6 documentés ; tête de vis non cotée. Ne pas la déclarer interchangeable sans ce contrôle. |

Voir [J5/C1/connecteurs](TECHNIQUE_J5_PASSIFS.md), [clips/D126](CLIPS_VS_D126.md) et [visserie](VISSERIE_GRAND_PUBLIC.md). Les fiches fabricant et pages marchandes directes sont identifiées dans [sources.json](sources.json), avec les faits réellement lus et leurs limites.

## Un ou deux fournisseurs électroniques

Montants pour **deux shields**, matériels réemployés exclus. Les ports TME et DigiKey ne sont comptés qu'une fois chacun. Les montants convertis sont des budgets, pas des devis France. Les prix/quantités vérifiés figurent dans [BOM_2_PROTOTYPES.csv](BOM_2_PROTOTYPES.csv) ; les autres postes V1.8 sont conservés dans [POSTES_RESTANTS.csv](POSTES_RESTANTS.csv).

| Scénario électronique | TTC avec ports France, indicatif | Disponibilité et décision |
|---|---:|---|
| TME seul, y compris les deux Connfly proposés | 18,81 € | **Contrefactuel non admissible** : J5 refusé et stock 1×9 zéro. Ce montant ne constitue pas un panier réalisable. |
| TME + DigiKey pour J5 Samtec seul, 1×9 Connfly attendus | 45,50–50,50 € | Aucun délai vérifié pour les 1×9 ; pas une solution rapide. |
| TME + DigiKey, J5 Samtec exact et quatre Harwin 1×9 | **49,88–54,88 €** | Scénario documenté avec stock affiché sur toutes les lignes ; C1/C2/Harwin restent des variantes conditionnelles. |
| Même scénario, avec un J5 et un Harwin de réserve | 52,68–57,68 € | Comparaison à réserves identiques avec V1.8 : 53,04–58,04 €, taux et prix différents. Gain limité à environ 0,36 €, pas une division du coût. |
| Scénario disponible conservant aussi le Kemet C1 actif | 50,25–55,25 € | Repli évitant une variante C1 ; surcoût d'environ **0,37 €** seulement pour trois pièces. |
| TME seul pour le reste, J5 et 1×9 exacts déjà possédés | 16,46 € de dépenses nouvelles | **Conditionnel** : stock personnel inconnu. Ne pas valoriser des pièces supposées présentes à zéro. |

**Recommandation économique :** vérifier le stock personnel avant toute dépense. À défaut, grouper J5 et les 1×9 chez le même distributeur ; ne pas lancer une expédition dédiée pour deux Samtec si les quatre 1×9 doivent aussi être achetés. Parmi les offres documentées et disponibles, DigiKey permet ce regroupement. Aucun minimum mondial de prix n'est revendiqué : Farnell et Mouser France n'ont pas été vérifiables, Samtec ne fournit pas de prix en ligne et RS propose un lot disproportionné. Une commande existante chez un distributeur pourrait changer le coût marginal, mais aucune n'est supposée.

L'économie AVX est minime. Pour préserver la référence active sans nouvelle condition d'assemblage, **conserver le Kemet est un repli raisonnable**, en acceptant les 0,37 € supplémentaires. La BOM KiCad n'a pas été changée.

## Budget complet : cible historique non démontrée

Calculs reproductibles dans [budget-TTC.json](budget-TTC.json). Conversion : `EUR HT = USD HT / 1,1224`, taux [BCE du 05/11/2026](https://www.ecb.europa.eu/stats/policy_and_exchange_rates/euro_reference_exchange_rates/html/eurofxref-graph-usd.en.html), puis TVA budgétaire 20 %. Ce taux n'est pas une garantie de facturation TME. Les lignes DigiKey sont lues directement en EUR, sans reconversion.

| Poste | Budget séparé et limite |
|---|---|
| Électronique disponible avec AVX | 49,88–54,88 € TTC avec deux transports. L'incertitude de 5 € vient du traitement TVA non résolu du port DigiKey affiché à 25 €. |
| Visserie Ajile, 8 vis + 16 écrous | 13,80 € TTC sans suivi ; **16,20 € avec suivi**. Tête de vis et tolérances à confirmer ; transport postal et délai différents. |
| Cinq PCB nus | Calculateur public V1.8 du **04/11**, réutilisé sans nouvel envoi : fabrication 5,30 USD ; ports 7,33/16,11/23,44 USD. À taux du 05/11 + TVA budgétaire : **13,50 / 22,89 / 30,73 €**. TTC/import final inconnu ; ce n'est pas un nouveau devis. |
| D126 et retenues modules | Fabrication/prestataire/coût réels inconnus ; 8 D126 dont 2 réserves. Quelques centimes de matière brute V2.1 ne valent pas un prix de réalisation. |
| Fixations modules, boîtiers, vitre, joints | Non chiffrés complètement ; voir postes restants. |
| Nucleo, modules, nappes, alimentation | Réemploi envisagé, inventaire de deux jeux non vérifié ; aucun achat neuf supposé inutilement. |

L'addition des seuls postes chiffrés **électronique + Ajile sans suivi + PCB lent estimé** donne déjà **77,18–82,18 €** ; **79,58–84,58 € avec Ajile suivi**. Ces sommes excluent les postes inconnus et restent conditionnelles aux offres et variantes. Le total de deux montages, cinq PCB et toute la mécanique est donc **inconnu**, et la cible historique d'environ 50 € n'est pas démontrée. Le risque prototype accepté ne change pas cette arithmétique et n'autorise aucune commande.

## Délais et premier exemplaire

TME annonce [2–4 jours ouvrables](https://www.tme.eu/fr/how-to-buy/7307/Moyens-et-couts-de-transport/) en économique selon conditions de paiement/expédition. DigiKey annonce généralement trois jours en France sur ses pages produit ; les 4/24 semaines fabricant ne sont pas les délais du stock affiché. Ajile annonce expédition sous 48 h après paiement, puis **3–5 jours ouvrés avec suivi ou 5–10 sans suivi**. PCB V1.8 : production annoncée 2 jours puis 10–12/6–9/2–4 jours selon transport. Aucun stock n'est réservé et aucune date d'arrivée n'est garantie.

Le seul contrôle d'assemblage indispensable pour le premier article est **à froid avec les pièces exactes** : tête et écrous conformes à l'enveloppe V2.1, G réel/parallelisme et engagements J5/Zio sans butée ni effort, D126 bien orientés et maintenus, queues recoupées sans contact parasite, isolement RS485 et câbles/modules/USER/LED dégagés. Aucune nouvelle impression papier demandée. Ce contrôle ne qualifie pas à lui seul fluage, vibration ou endurance et ne vaut pas permission de fabriquer.

## Reproduction et conservation

Depuis la racine du worktree :

```bash
python3 'Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_2_ECONOMIE/tools/verify_study.py'
```

Le script recalcule devis partiels/TVA, vérifie les quantités et prix, contrôle les conclusions localisées et compare les **486 fichiers antérieurs du shield octet pour octet** au HEAD initial `6542a4a5fdc970f3467658cc151e710677633d6f`. Il ne contacte aucun matériel et n'effectue aucune commande commerciale. Résultats dans `preuves/`. ERC/DRC, cross-build et essais acquis non répétés : aucune source électrique ni firmware modifiée. La publication distante est tracée dans le rapport de mission.

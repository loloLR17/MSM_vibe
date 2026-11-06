# Morvan — TR2 V2.7, ABS uniquement, deux prototypes

**À produire après revue indépendante du dossier par ChatGPT. Ne pas imprimer le lot V2.6.** Aucun achat ou envoi réalisé par Codex.

Ouvrir d'abord `vues/VUE_ECLATEE_V27.png` et `vues/PLAN_MONTAGE_V27.pdf`, puis suivre `QUANTITES.csv` :25 fichiers STL, **61 pièces en tout**. Chaque STL contient UNE pièce, pas un plateau complet. Respecter la colonne quantité ; ne pas multiplier tout le ZIP par deux.46 pièces sont montées,12 alternatives,2 D126 de réserve et1 coupon. Ne pas imprimer les STEP/PDF/PNG, qui servent à comprendre et éditer.

ABS non chargé électriquement conducteur, une seule couleur possible. Les couleurs des vues ne sont pas des couleurs d'impression imposées. STL en millimètres, échelle100 %, déjà orientés : `flat` à plat, `north/east` sur leur face de panneau, `upside_down` retournés. Aucun besoin de retoucher la CAO.

## Profil raisonnable, à adapter à la machine

- Profil ABS éprouvé du filament et de l'imprimante, enceinte/adhésion selon les instructions du fabricant. Pas de température ni temps garanti sans connaître la machine.
- Base/capot158×166mm : vérifier le plateau **avec la bordure d'adhésion** dans le trancheur ; ne pas supposer sa taille. Hauteur de caisse61mm, boîtier fermé64mm/65mm avec relief. Éviter le gauchissement des grandes faces.
- Grands volumes : couches0,15–0,20mm, quatre périmètres,25–35 % de remplissage comme point de départ. Selles/cage/patins/poussoirs :100 %, couches0,10–0,15. D126 :100 %, couches0,10mm.
- Supports locaux sous les débords de la collerette des poussoirs et si nécessaire dans les poches. Ils doivent être entièrement retirables ; ne pas emprisonner des supports dans les alésages. Les étriers/patins sont livrés retournés pour réduire les supports.
- Imprimer d'abord l'unique coupon50 : trous2,4/3,6/4,8/13, écrou AF6,1, témoins12,25 et11,2. Ajuster le profil/compensation d'alésage selon mesure, pas une mise à l'échelle aveugle de tout le boîtier qui déplacerait les axes ST.
- Aléser/ébavurer légèrement si nécessaire, loin de l'électronique. Aucun filetage plastique à tarauder : tous les filetages utiles sont métalliques. Guides5/5,6 et tige4/guide4,8 ; poches AF6,1. Ces jeux de conception ne garantissent pas le retrait d'une machine donnée.

## Tri simple

Les selles H10,6/H11,2/H11,8 et les tiges Z8,8/Z9,6/Z10,4 portent respectivement1/2/3 petits creux intégrés. Livrer les variantes séparées des pièces utiles et des deux D126 de réserve. Le coupon ne se monte pas. N'imprimer aucune rondelle/cale supplémentaire : les24 rondelles isolantes sont des fournitures nylon standard listées dans la BOM.

Après ébavurage, **D126 H12,15..12,35mm** ; la cote fonctionnelle G12,25±0,10 n'est pas assouplie pour l'ABS. Vérifier aussi appuis plats, patins coulissants, maintien des écrous et retour USER à froid. Une retouche locale est permise ; ne jamais forcer des axes ni fléchir une carte.

Le premier montage à froid décrit dans `MONTAGE_A_FROID.md` est indispensable. Dossier de prototype, sans qualification industrielle, vibration ou IP et sans promesse de masse/temps du trancheur. Pas d'impression engagée automatiquement.

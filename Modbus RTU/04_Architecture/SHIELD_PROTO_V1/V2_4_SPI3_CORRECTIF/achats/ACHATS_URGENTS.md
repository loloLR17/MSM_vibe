# Préparation urgente pour deux shields — aucun achat lancé

**Checkpoint du18/11/2026.** La correction SPI3 ne change ni le nombre ni les valeurs des composants. La liste CSV distingue les références actives de la CAO, les achats historiques et les alternatives **conditionnelles**, qui ne sont pas adoptées en silence. Aucun compte fournisseur, panier authentifié ou paiement utilisé. Les prix internationaux USD HT ne constituent pas un devis TTC France.

## Action utile avant toute dépense

Inventorier les pièces **non soudées** de la commande TME historique de17,60€TTC. Les dix1x12, dix1x10 et dix1x8 annoncés ne signifient pas qu'il faut racheter dix de chaque : quatre de chaque famille suffisent pour deux shields si la variante1x10 est réellement acceptable. L'hypothèse six restants après quatre utilisés n'est **pas un inventaire**. La question de stock a été transmise ; la CAO continue sans attendre.

**Ne pas racheter** les deux Nucleo, FRAM, lecteursSD, kitsMKIGIBV2/MKI208V1, Click ou cartesSD pour cette correction. Réutiliser les modules enfichables, leurs données et les pièces mécaniquesV2.7 ; état physique ultérieur à contrôler séparément. Ne rien dessouder maintenant. Pour chiffrer prudemment le PCBneuf, considérer ses connecteurs soudés actuels comme non récupérables.

## Besoin prudent et lignes prioritaires

- J1..J4 : trois barres mâles2x40 `DS1021-2*40SF11` fournissent120colonnes,100utiles pour deux jeux10/8/15/17. StockTME affiché48700, MOQ2/multiple1 ; trois à0,60USDHT.
- J8 : quatre femelles1x12 `DS1023-1*12S21`, lot10 imposé, stock8520,0,14USDHT/unité. J9 : quatre1x8 `DS1023-1*8S21`, lot10, stock33350,0,11USDHT/unité. Liens exacts dans le CSV.
- J5 : deux **Samtec SSW-103-01-F-D exacts**. Priorité au stock d'échantillons déjà reçu et non monté ; délais d'échantillons non maîtrisés. Les pagesDigiKey ont refusé la relecture de stock et Mouser n'offre pas de confirmation fraîche exploitable. Pas de promesse avantvendredi. Ne pas remplacer par un connecteur de même hauteur sans zone utile d'insertion documentée.
- J6/J7 : quatre1x9 Neltron `2212S-09G-85`, référence active. Harwin `M20-7820946` est une alternative nominale documentée, mais stock/délai actuels non confirmés. Les quatre1x10 Connfly historiques `DS1023-1*10S21` sont affichées disponibles45440, lot10 à0,13USDHT ; **leur corps et dixième contact demandent une vérification M-01**, pas un simple changement de quantité. Aucune substitution adoptée à ce checkpoint.
- Passifs : huit10k `MRS16000C1002FCT00` (palier10 à0,16USDHT, stock978) ; deux100nF et deux10µF. Références actives et alternatives historiques AVX/Panasonic distinctes dans le CSV. Les variantes conditionnelles ne deviennent pas des composants qualifiés par leur stock affiché.

Un lot prudent TME comprenant3barres +10femelles12 +10femelles8 +10résistances coûte **5,90USDHT hors transport/taxes**, selon les pages consultées. Ajouter1,30USDHT pour10femelles10 uniquement si cette variante est acceptée. C1/C2 et J5 restent à compléter depuis stock réel ou références compatibles contrôlées. Ce sous-total n'est pas un prix complet de deux shields.

## Échéance avant samedi21novembre

[Tarif public TME France](https://www.tme.eu/fr/how-to-buy/7307/Moyens-et-couts-de-transport/) : économique7,08€TTC, délai estimé2–4jours ouvrables ; UPSExpressSaver/DHLExpress/FedExPriority19,08€TTC jusqu'à10kg, délai estimé1–2jours ouvrables si commande avant14hCET. Ce sont des estimations, pas une garantie à une adresse.

Au soir du mercredi18, une commande jeudi19 avant14h pourrait arriver vendredi20 en scénario1jour, **ou lundi23 en scénario2jours**. L'objectif « avant samedi21 » n'est donc pas assuré. L'économique ne constitue pas un plan fiable pour cette date. PourDigiKey, la pageFrance consultable annonce typiquement3jours et25€deport sous75€ ; pas de réception avantvendredi démontrée, traitementTVAduport non confirmé. Ne pas engager de commande sur la base d'une vieille capture de stock ou d'un délai fabricant en semaines confondu avec un stock distributeur.

Les sources factuelles, devises, réserves et échecs d'accès sont consignés dans SOURCES_COMMERCIALES.json. La libérationPCB exige toujours E-01/M-01 et revueChatGPT ; aucunZIP n'est envoyé au fabricant par cette préparation.

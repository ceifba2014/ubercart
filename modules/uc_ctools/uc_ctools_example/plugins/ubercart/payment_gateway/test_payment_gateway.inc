<?php

/**
 * @file
 * A payment method plugin for Ubercart.
 */

/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'title' => t('Test Payment Gateway'),
  'description' => t('Process credit card payments through the Test Payment Gateway.'),
  'credit' => 'uc_ctools_example_charge',
);

/**
 * Callback function to perform the charge operation.
 *
 * @see test_gateway.module
 * @see test_gateway_uc_payment_gateway()
 */
function uc_ctools_example_charge($order_id, $amount, $data) {
  global $user;
  $order = uc_order_load($order_id);

  // cc_exp_month and cc_exp_year really should be validated in uc_credit.
  $month = $order->payment_details['cc_exp_month'];
  $month = $month % 12;

  $year  = $order->payment_details['cc_exp_year'];
  if ($year < 100) {
    $year = $year + 2000;
  }

  // Expires on last day of month.
  $expiration_date = mktime(0, 0, 0, $month + 1, -1, $year);

  // Conditions for failure are described in file documentation block above.
  // All other transactions will succeed.
  if ($order->payment_details['cc_number'] == '0000000000000000' ||
      $order->payment_details['cc_cvv'] == '000'                 ||
      ($expiration_date - time()) <= 0                           ||
      $amount == 12.34                                           ||
      $order->billing_first_name == 'Fictitious'                 ||
      $order->billing_phone == '8675309'                            ) {
    $success = FALSE;
  }
  else {
    $success = TRUE;
  }

  // Uncomment this line to see the order object.  The information for the
  // payment is in the $order->payment_details array.
  // drupal_set_message('<pre>' . print_r($order->payment_details, TRUE) . '</pre>');

  if ($success) {
    $message = t('Credit card charged: !amount', array('!amount' => uc_currency_format($amount)));
    uc_order_comment_save($order_id, $user->uid, $message, 'admin');
  }
  else {
    $message = t('Credit card charge failed.');
    uc_order_comment_save($order_id, $user->uid, $message, 'admin');
  }

  $result = array(
    'success' => $success,
    'comment' => t('Card charged, resolution code: 0022548315'),
    'message' => $success ? t('Credit card payment processed successfully.') : t('Credit card charge failed.'),
    'uid' => $user->uid,
    // 'data' => $data,
  );

  return $result;
}